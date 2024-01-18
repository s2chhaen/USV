function jsonToCHeader()
	function appendStrFile(str)
		l = length(strFile);
		strFile(l+1) = str;
	end
	dataBlockFileName = "datablock.h";
	dataBlockPath = "..\00 Bussystem\" + dataBlockFileName;
	
	try
		usvAddress = importUSVaddresses();
	catch err
		rethrow(err);
	end
	
	[strH,strM,strF] = load_hTemplate("datablockTemplate.h");
	AddrNames = string(fieldnames(usvAddress));
	
	strFile = strH;
	
	% set filename
	strFile(2) = sprintf(strFile(2),dataBlockFileName);
	
	% set file Date
	TF = contains(strFile,"Created:");
	DV = datevec(now);
	strFile(TF) = sprintf(strFile(TF),DV(3),DV(2),DV(1),DV(4),DV(5));


	% get version from old file
	strArrOldDB = readlines(dataBlockPath,"LineEnding","\r\n");
	TF = contains(strArrOldDB,"- data rev.");
	strLine = strArrOldDB(TF);
	version = nan;
	if ~isempty(strLine)
		res = strsplit(strLine," ");
		version = str2double(res(4));
	end
	if isnan(version)
		version = 1;
	else
		version = version+1;
	end
	
	% set version
	TF = contains(strFile,"- data rev.");
	strLine = strFile(TF);
	strLine = sprintf(strLine,version);
	strFile(TF) = strLine;

	nTabs = sum(double(char(strH(end)) == sprintf("\t")))+1;
	Tabs = string(repmat(sprintf("\t"),1,nTabs));
	for AddrName=AddrNames'
		C = usvAddress.(AddrName);
		Spaces = string(repmat(' ',1,5-length(char(AddrName))));
		appendStrFile(sprintf("%s%s%s= 0x%04X,",Tabs,AddrName,Spaces,C{2}));
	end

	strFile = [strFile;strM];

	nTabs = sum(double(char(strH(end)) == sprintf("\t")))+1;
	Tabs = string(repmat(sprintf("\t"),1,nTabs));
	for AddrName=AddrNames'
		C = usvAddress.(AddrName);
		Spaces = string(repmat(' ',1,5-length(char(AddrName))));
		appendStrFile(sprintf("%s%s_len%s= %03d,",Tabs,AddrName,Spaces,C{3}));
	end

	strFile = [strFile;strF];

	fid = fopen(dataBlockPath,"w");
	if fid ~=0
		try
			fprintf(fid,"%s\r\n",strFile);
			fclose(fid);
		catch
			fclose(fid);
		end
	end
	
end

function [strHeader,strMid,strFooter] = load_hTemplate(templateName)
	try
		strArr = readlines(templateName,"LineEnding","\r\n");
	catch err
		rethrow(err);
	end
	strim = strtrim(strArr);
	TFaddr = matches(strim,"/*address*/");
	TFlen = matches(strim,"/*lenght*/");
	addrLine = find(TFaddr);
	lengthLine = find(TFlen);

	strHeader = strArr(1:(addrLine-1));
	strMid = strArr((addrLine+1):(lengthLine-1));
	strFooter = strArr((lengthLine+1):length(strArr));
end
