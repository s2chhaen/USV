program USVMonitor;

uses
  Forms,
  Monitor in 'Monitor.pas' {Form1};

{$R *.RES}

begin
  Application.Initialize;
  Application.Title := 'USV-Monitor';
  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.
