function jsonToCHeader()
	dataBlockFileName = "datablock.h";
	dataBlockPath = "..\00 Bussystem\" + dataBlockFileName;
	headerSaveDefine = "DATABLOCK_H_";
	enumeratorName = "Datablock";
	templateHFile = "datablockTemplate.h";

	function appendStrFile(str)
		l = length(strFile);
		strFile(l+1) = str;
	end
	function str = replaceStr(str,str_find,str_rep)
		TFl = contains(str,str_find);
		strLinel = str(TFl);
		strLinel = strrep(strLinel,str_find,str_rep);
		str(TFl) = strLinel;
	end

	try
		usvAddress = importUSVaddresses();
	catch err
		rethrow(err);
	end

	try
		strArr = readlines(templateHFile,"LineEnding","\r\n");
	catch err
		rethrow(err);
	end

	% set filename
	strArr = replaceStr(strArr,"/*filename*/",dataBlockFileName);
	
	% set file Date
	DV = datetime;
	DV.Format = 'dd.MM.uuuu HH:mm:ss';
	strArr = replaceStr(strArr,"/*datetime*/",string(DV));

	% get version from old file
	strArrOldDB = readlines(dataBlockPath,"LineEnding","\r\n");
	TF = contains(strArrOldDB,"- data rev.");
	strLine = strArrOldDB(TF);
	version = nan;
	if ~isempty(strLine)
		res = strsplit(strLine," ");
		version = str2double(res(end));
	end
	if isnan(version)
		version = 1;
	else
		version = version+1;
	end
	
	% set version
	strArr = replaceStr(strArr,"/*version*/",sprintf("%d",version));

	% set header save define 
	strArr = replaceStr(strArr,"/*headerSaveDefine*/",headerSaveDefine);

	% set enumerator names 
	strArr = replaceStr(strArr,"/*addressEnum*/",enumeratorName);

	% split template
	[strH,strM,strF] = load_hTemplate(strArr);

	strFile = strH;
	
	% get Address names
	AddrNames = string(fieldnames(usvAddress));

	% copy addresses into the enumerator define
	nTabs = sum(double(char(strH(end)) == sprintf("\t")))+1;
	Tabs = string(repmat(sprintf("\t"),1,nTabs));
	for AddrName=AddrNames'
		C = usvAddress.(AddrName);
		Spaces = string(repmat(' ',1,5-length(char(AddrName))));
		appendStrFile(sprintf("%s%s%s= 0x%04X,",Tabs,AddrName,Spaces,C{2}));
	end

	% append mid segment
	strFile = [strFile;strM];

	% copy length into the enumerator define
	nTabs = sum(double(char(strH(end)) == sprintf("\t")))+1;
	Tabs = string(repmat(sprintf("\t"),1,nTabs));
	for AddrName=AddrNames'
		C = usvAddress.(AddrName);
		Spaces = string(repmat(' ',1,5-length(char(AddrName))));
		appendStrFile(sprintf("%s%s_len%s= %03d,",Tabs,AddrName,Spaces,C{3}));
	end

	% append footer segment
	strFile = [strFile;strF];

	% write file
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

function [strHeader,strMid,strFooter] = load_hTemplate(strArr)
	strim = strtrim(strArr);
	TFaddr = matches(strim,"/*address*/");
	TFlen = matches(strim,"/*lenght*/");
	addrLine = find(TFaddr);
	lengthLine = find(TFlen);

	strHeader = strArr(1:(addrLine-1));
	strMid = strArr((addrLine+1):(lengthLine-1));
	strFooter = strArr((lengthLine+1):length(strArr));
end
