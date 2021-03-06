//<Vars>
Var
  MeasTime	  : Real;       //����� ���������.
  IntStop,IntTact : Integer;	//������������ ��� ������������ ��������� ���������� ��.
  AllPins	  : TLargeSet;	//AllPins - ������ ������ ���� ������� ����������, ����� ������� � �����.  
  ActiveBoard	  : Integer;	//����� ������ �� ������������ ���-����.
  DeviceNotReady  : Boolean;	//���������� ��� ������ � ������ ������������� �� �����������  ������������ ��� ��� �������������������.
  DeltaBoard	  : integer;	//��� �����������. �������� ������ �����.
  DeltaH	  : integer;	//��� �����������. �������� ������ ����.

  PWR		  : tNamePower;	//{pVCC, pVEE, pVDD, pDCM} - ��������� ��������. ��������� ������ ������������ � ������ ������ �������� �������.
  PWROLD	  : tNamePower;	//��������� ���������� �������� ���������� PWR, ��� ������������ ��������������. ������������ � ��������� � �������� ICC.
  ComDCMPWr	  : TEDCMComm;	//{DCM_off, DCM_To_VEE,DCM_To_VCC,DCM_To_VDD,DCM_To_Pins} - ��������� ��������. ����������� DCM � ��������� �������.
  GND		  : tNamePower;	//���������� ��������� ��������� ����� (���� ��� ������������). ���������� �� ������ ������ ������������ ������ VEE.
  ComDCMGND	  : TEDCMComm;	//����������� DCM � ��������� �����.
  VEEON		  : Boolean;	//���������� ������ ���������� �� ������������� �������� �����.
  SVPN2		  : Boolean;	//���������� ������������ ������������ �������� SVPN. �� ������ ������ ��������� ������ ��� 1053 �������.

  First_Run	  : Boolean;	//������ ���������� ������ �������� � �����.
  Last_Run	  : Integer;	//���������� ������������ ��������� ����������� ������.
  K		  : Integer;	//������������ ��� ������� � ����� ��� ���������������� ������ ������� �������.
//<Vars>


//<Const>
Label
  //����� ��� ������ �� ��������� � ������ ������������� �� ������������/����������� ������������
  ProgrammBrake;

Const
  //�������� ��� �������� �������
  Pico  = 1.0e-12;
  Nano  = 1.0e-9;
  Micro = 1.0e-6;
  Milli = 1.0e-3;
  Uno   = 1.0e-0;
  //��������� �������� ��������� ����������, ����� ���������� � �������� ���������� ���������
  Upwr_0 : Real	= 0;
  Ugnd_0 : Real	= 0;
  DRH_0	 : Real	= 0;
  DRL_0	 : Real	= 0;
  DRH1_0 : Real	= 0;
  DRL1_0 : Real	= 0;

  FullText : PChar = '';	//���������� ����������� ����� ��������������� ���������
  TesNum   : Integer = 138;	// ����� �������

  IMSNumber    : Integer = 0;			//����� ����������� ��, ��� ������ ����������
  TestNumber   : Integer = 0;			//����� �����, ��� ������ ����������
  ShortName    : String  = '';			//������ � �������� ������ �����
  PowerComment : String  = '';			//������ � ����������� �������
  PNum	       : String  = '';			//������ � ������ � ������� ������������ ������
  WTFVar       : Real    = 0.0;			//������� ��� �������
  ConnPins     : TLargeSet = [];		//������ ������������ � �������� UO/ILI, ��� ������������� ������������ ����� ������� �������
//<Const>


//<Dllfunc>
Procedure Initialise(FName, PPVersion, MadeDate, DesignerName : String; testerNumber : integer; Var IMSNumber : integer); stdcall; external 'PrintResult.dll' name 'Init';
Procedure Header(FName : String; Var IMSNumber : integer; DebugMode : boolean); stdcall; external 'PrintResult.dll' name 'HeadPrint';
Procedure SaveResult(FName, TName : String; Var PowComm : String; Var PNum : String; Var ShTestName : String; PinName : String; PinNum : integer; UccValue : real; Var Value : real; var LBound : real; var UBound : real; dim, tin : real; Var IMSNumber : integer; Var testNum : integer); stdcall; external 'PrintResult.dll' name 'Print1';	
Procedure Repin(PinS, Adap60 : boolean; TesterNumber : integer; Var BoardChange : integer; Var PinChange : integer; Var InfoText : String; Adapter, Kontakt, Jumpers, Pin2 : String); stdcall; external 'Repin.dll' name 'Repin';
//<Dllfunc>


//<Basefunc>

//<Printrestu>
//���������� �������� � �������� ��
//PrintResTU(TestName, PinNum, Upwr, Value, LowerBound, UpperBound, Dim);
Procedure PrintResTU(TestName : PChar; PinNum : integer; Upwr, Value, LBound, UBound, Dim: Real);
  Begin
    if PinNum <> -1 then
      SaveResult(FileName, TestName, PowerComment, PNum, ShortName, NamePin[PinNum], PinNum + 1, Upwr, Value, LBound, UBound, Dim, Tin, IMSNumber, TestNumber);
    else
      SaveResult(FileName, TestName, PowerComment, PNum, ShortName, '', PinNum, Upwr, Value, LBound, UBound, Dim, Tin, IMSNumber, TestNumber);
    PrintResult(ShortName, PNum, Value, LBound, UBound, PowerComment);
  End;
//<Printrestu>

//<Updown>
//������� ��������� �������
//���������� ��� ���������� �������� ��� ��������� ���������� ������� � ���������
//UPWR,UGND,DRH,DRL,DRH1,DRL1-�������� ���������� dU-��� � ������� ���������� � ����� �����������
//UpDown(Boards, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
Procedure UpDown(Var Boards : TLargeSet; Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU : Real);
  Var
    J  : Integer;
    //��������� ��������� ����������
    delta, deltaP, deltaG, deltaDRH, deltaDRL, deltaDRH1, deltaDRL1 : Real;
  Begin
    //������� ������� ����� �������� � ���������� ��������
    deltaP    := Upwr - Upwr_0;
    deltaG    := Ugnd - Ugnd_0;
    deltaDRH  := DRH  - DRH_0;
    deltaDRL  := DRL  - DRL_0;
    deltaDRH1 := DRH1 - DRH1_0;
    deltaDRL1 := DRL1 - DRL1_0;

    //���������� ������������ ������� ����������
    if abs(deltaP) < abs(deltaG) then
      delta := abs(deltaG);
    else
      delta := abs(deltaP);

    if delta < abs(deltaDRH) then
      delta := abs(deltaDRH);

    if delta < abs(deltaDRL) then
      delta := abs(deltaDRL);

    if delta < abs(deltaDRH1) then
      delta := abs(deltaDRH1);

    if delta < abs(deltaDRL1) then
      delta := abs(deltaDRL1);

    //������������ ����������� ����� �����
    J := trunc(delta/dU);
    
    //��� ������������ �������� ���������� ������ ����
    if J > 0 then
      begin
        //������������ ����������� ��������� ���������� �� 1 ��������
        deltaP    := deltaP    / J;		
	deltaG    := deltaG    / J;		
	deltaDRH  := deltaDRH  / J;		
	deltaDRL  := deltaDRL  / J;		
	deltaDRH1 := deltaDRH1 / J;		
	deltaDRL1 := deltaDRL1 / J;	
	
	//��������� ������������ ���� ��������� ����������
	for K:=1 to J do
	  begin				
	    //��������� ����� �������� ����������
	    Upwr_0 := Upwr_0 + deltaP;
	    Ugnd_0 := Ugnd_0 + deltaG;
	    DRH_0  := DRH_0  + deltaDRH;
	    DRL_0  := DRL_0  + deltaDRL;
	    DRH1_0 := DRH1_0 + deltaDRH1;
	    DRL1_0 := DRL1_0 + deltaDRL1;

	    //������ ������� ����������
	    SetPowerU(PWR, Upwr_0);	
	    DelayMS(TimeUSB);

	    SetPowerU(GND, Ugnd_0);	
	    DelayMS(TimeUSB);
	    //������ ������ ���������� �� ���������
	    LoadDACPinsLevels(Boards, DRH_0, DRL_0, (Upwr_0 - Ugnd_0)/2+1, (Upwr_0 - Ugnd_0)/2-1, DRH1_0, DRL1_0);//��������� ����� ������� U.
	    DelayMS(TimeUSB);
	  end;
      end;

    //����� ������ �� ����� ��� ��� ������������ ��������� ������� ������ ����
    //������������ ������ ��������� ������ ���������� �� ���������
    LoadDACPinsLevels(Boards, DRH, DRL, (Upwr - Ugnd)/2+1, (Upwr - Ugnd)/2-1, DRH1, DRL1);
    DelayMS(TimeUSB);
    //� �� ����������
    SetPowerU(PWR, Upwr);
    DelayMS(TimeUSB);

    SetPowerU(GND, Ugnd);
    DelayMS(TimeUSB);

    //���������� ������� �������� ���������� � ���������� ��� ������������� � ����������
    Upwr_0 := Upwr;
    Ugnd_0 := Ugnd;
    DRH_0  := DRH ;
    DRL_0  := DRL ;
    DRH1_0 := DRH1;
    DRL1_0 := DRL1;
  End;
//<Updown>

//<Unconnectalll>
//������� ����������
// ���������� ��� ��������� ������� ���� � ���������� � ������� ��� ����������.
Procedure UnConnectAlll(us0, us1 : Integer);
  Var
    S: Integer;
  Begin
    //���������� ������ �������� �������� ��� ���� ����
    TERminationMode ([1..16], TermOFF, 0.0, 0.0, 0.0);
    DelayMs(TimeUSB);
    
    //���������� ������� ������ ������ ��������� ��� ���� �������
    conf_levels_hand(AllPins,DNRZ,[]);
    DelayMs(TimeUSB);

    //������� �� ���� ����� ����� USER_0 �����.
    PRGUser(UserPort_0, us0);
    DelayMs(TimeUSB);

    //������� �� ���� ����� ����� USER_1 �����.
    PRGUser(UserPort_1, us1);
    DelayMs(TimeUSB);
	
    //����� ������� UpDown ��� �������� ��������� ���������� �� ������� � ���������.
    UpDown(BoardList, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.25);
	
    //��������� ��� ���� ��� 16 ����
    for K := 0 to 15 do
      begin
	SendPat(K, 0, 65535);
	DelayMs(TimeUSB);
      end;

    //��������� ��� ���� � ������
    ConnectPins([1..256], []);
    DelayMs(TimeUSB);

    //��������� ��� ��������� ��� ���� ���� SVPN
    for S := 1 to 4 do
      begin
        ActivateSVPN(S);
        DelayMs(TimeUSB);

        SetDCM(0.0, R8, 1.0e-6, DCM_1);
        DelayMs(TimeUSB);
		
	ConnectDCM(DCM_Off);
	DelayMs(TimeUSB);
		
	SetPower(pVCC, 0.0, 1.0e-6, Source_off);
	DelayMs(TimeUSB);
		
	SetPower(pVDD, 0.0, 1.0e-6, Source_off);
	DelayMs(TimeUSB);
		
	SetPower(pVEE, 0.0, 1.0e-6, Source_off);
	DelayMs(TimeUSB);
      end;

    //������������ ������� "���������� �����" �� ����-�
    UnConnectAll;
    DelayMs(TimeUSB);
  End;
//<Unconnectalll>

//<Raportprint>
//��������� ���������� ����� ������������, ���� ��������� � ������ ��
Procedure RaportPrint(MeasTime : real);
  Begin
    SaveResult(FileName, '����� ���������, c', PowerComment, PNum, ShortName, '', -1, NULL, MeasTime, MeasTime, MeasTime, Uno, Tin, IMSNumber, TestNumber);
    PrintResult('����� ���������, c', '', MeasTime, MeasTime, MeasTime, '');
    PrintResult(Data,'', 0, 0, 0, '');
    PrintResult('����������� ��:', '', 0, 0, 0,'');
    PrintResult(Raz, '', 0, 0, 0, '');
    PrintResult('��:', '', 0, 0, 0, '');
    PrintResult(TU, '', 0, 0, 0, '');
    PrintResult(Ver, '', 0, 0, 0, '');
  End;
//<Raportprint>

//<Deltaproc>
//������� ��������� �������� �����
Procedure DeltaProc(Var DeltaSet : TLargeSet; Displacement : Integer);
  Var
    TempSet : TLargeSet;
    I : integer;
  Begin
    TempSet := [];
    for I := 1 to 256 do
      begin
        if (I in DeltaSet) then
	  begin
	    exclude(DeltaSet, I);
	    include(TempSet, I + Displacement);
	  end;
      end;
    deltaSet := TempSet;
  End;
//<Deltaproc>

//<Basefunc>


//<Pinchangedll>
//������� ����������� � �������������� ������� ���������
Procedure PinChangeDll(Var x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12 : TLargeSet);
  Begin
    //��������� ������ �������
    if DEBUG = false then
      TesNum := GetTesterNumber // ��������� ����� �������
    else
      TesNum := StrToInt (InputBox('������� ����� �������'));

    //�����������, PinS, Adap60 - bool, DeltaBoard, DeltaH - ����������, ���������� ��������
    //FullText - ���������� ��� ����������� ������ ������ ���������
    //Adap, KU, Jamper, Pin2 - ���������� � ����������� � ����������� ��������
    Repin(PinS, Adap60, TesNum, DeltaBoard, DeltaH, FullText, Adap, KU, Jamper, PinLast);

    //������� ���� ������ � ����������� �� ������ ������������ H-�����
    if DeltaBoard = 2 then
      StrConcat(FileName, '_1029_1033');

    if DeltaBoard = 4 then
      StrConcat(FileName, '_135_1053');
 
    if DeltaBoard = 6 then
      StrConcat(FileName, '_adap60');
  
    if DeltaBoard = 9 then
      StrConcat(FileName, '_148');
  
    //����� ������ ���� ��������� �� ����������� �� ������ �������
    if DeltaBoard = -1 then
      begin
        ErrorBox(FullText, '������');
        DeviceNotReady := true;
      end
    else
      begin
      //����������� �������� ������������ ���-����, ������� � �������� �����
      //DeltaProc(DeltaSet, Displacement);
        DeltaProc(BoardList, DeltaBoard);
        DeltaProc(InPins, DeltaH);
        DeltaProc(OutPins, DeltaH);
//<Deltaprocpart>
        //����������� ���� �������, ���������� ����������� ��� ������ ��
        DeltaProc(x1, DeltaH);
        DeltaProc(x2, DeltaH);
        DeltaProc(x3, DeltaH);
        DeltaProc(x4, DeltaH);
        DeltaProc(x5, DeltaH);
        DeltaProc(x6, DeltaH);
        DeltaProc(x7, DeltaH);
        DeltaProc(x8, DeltaH);
        DeltaProc(x9, DeltaH);
        DeltaProc(x10, DeltaH);
        DeltaProc(x11, DeltaH);
        DeltaProc(x12, DeltaH);
//<Deltaprocpart>

        //����������� ������� � ���������� ���� � ������� ���-�����
        FirstPin    := FirstPin   + DeltaH;
        LastPin     := NumOfPin   + DeltaH;
        ActiveBoard := DeltaBoard + 1;	

        //����� ��������� � ����������� ��������
        InfoBox(FullText, FileName);
        DeviceNotReady := false;
      end;
  End;
//<Pinchangedll>


//<Contact>
//5
//Procedure Contact(Var MeasPin : TLargeSet; UDCM, IDCM, LBound, UBound : Real);
//AllPins; ������ �������������� �������
//-2.0; ���������� DCM, �
//10.0e-6; ��� DCM, �
//-1.5; ������ �������, �
//-0.3; ������� �������, �
//�������� ��������������� � ��
//�� ��
//������ ����� ��� �������� � GND ����� ����. ������ �������������� ���������� ��������� ����, �������� ������� U �� ����� (LB<U<UB).
//� ������ U > UpperBound ����� �� (���� ������, ������������� ���������� �������� 0).
//� ������ U < LowerBound ����� ����� - ����� ������� �� ��������� � ������������ ������ ���.
//        Contact( MeasPin, UDCM, IDCM, LBound, UBound, NamePWR);
Procedure Contact(Var MeasPin : TLargeSet; UDCM, IDCM, LBound, UBound : Real);
  Var
    Ucont : Real;
  Begin
    
    //������������� ������ ������� ������� � ���� �� DCM
    SetDCM(UDCM, RAuto, IDCM, DCM_Auto);             
    DelayMS(TimeUSB);

    //����������� DCM � ��������� �������
    ConnectDCM(ComDCMPwr);                 
    DelayMs(TimeMeasU);                    

    //��������� ����������
    Ucont := OnlyMeasDCMVoltage(ADCRange_AUTO); 

    //��� ����� ������� ��������� �� �����
    if (Ucont > UBound) or (Ucont < LBound) then
    //PrintResTU (         TestName,             PinNum, Upwr, Value, LBound, UBound, Dim);
      PrintResTU ('���������������', LastPin - FirstPin, NULL, Ucont, LBound, UBound, Uno);
    //����� ����� ���������� ������ �� ������� ������
    else
      SaveResult(FileName, '���������������', PowerComment, PNum, ShortName, NamePin[LastPin - FirstPin], LastPin - FirstPin + 1, NULL, UCont, LBound, UBound, Uno, Tin, IMSNumber, TestNumber);

    //���������� DCM � ������������� ����
    ConnectDCM(DCM_To_Pins);
    DelayMs(TimeUSB);
    
    //���� �� ���� �������
    for K := FirstPin to LastPin do
      begin
        //������� �������� ���� ����� �� � ������ ����������
        if not(K in MeasPin) then continue;

	//����������� ������ � DCM
        ConnectPinN(K, [Pin_To_DCM]);
        DelayMs(TimeMeasU);

	//��������� ������� ����������
        Ucont:= OnlyMeasDCMVoltage(ADCRange_AUTO);

	//��������� ����� � ������
        ConnectPinN(K, []);
	DelayMS(TimeUSB);

	//��� ����� ������� ��������� �� �����
        if (Ucont > UBound) or (Ucont < LBound) then
        //PrintResTU(         TestName,       PinNum, Upwr, Value, LBound, UBound, Dim);
	  PrintResTU('���������������', K - FirstPin, UDCM, Ucont, LBound, UBound, Uno);
	//����� �������� ��������� ������ �� ������� ������
	else
          SaveResult(FileName, '���������������', PowerComment, PNum, ShortName, NamePin[K - FirstPin], K - FirstPin + 1, NULL, UCont, LBound, UBound, Uno, Tin, IMSNumber, TestNumber);
      end;

    SetDCM(NULL, RAuto, IConnect, DCM_1);
    DelayMS(TimeUSB);

    ConnectDCM(DCM_Off);
    DelayMS(TimeUSB);

  End;
//<Contact>


//<Uo>
//15
//Function UO(Var MPins : TLargeSet; Upwr, Ugnd, DRL, DRH, DRL1, DRH1, ILimDCM, UDCM, LBound, UBound : Real; StartFK : Integer; Var  StopFK : TVectors; TestName : String; Mode : Integer) : Real;
//OutPins; ������ ����������� �������
//5.0; ���������� �������, �
//0.0; �������� �����, �
//0.4; ���������� ������� ������, �
//4.5; ���������� �������� ������, �
//0.0; ���.���������� ������� ������, �
//0.0; ���.���������� �������� ������, �
//1.3e-3; �������� ���, �
//2.5; �������� ���������� ����������, �
//0.8; ������ �������, �
//4.0; ������� �������, �
//0000; ��������� ������ FK
//VectorUOH; �������� ������ FK
//1.����.���.��.UOH,B?1. ���������� �������� ������ UOH, �; ��� �����
//0000; ��������� ������ (1 ������ - ������������ ����� (0 - ���, 1 - ����), 2 ������ - ����������� DCM (0 - �� �����, 1 - �����), 3 ������ - ������������� (0 - ���, 1 - ����), 4 ������ - ����������� �������� ����� (0 - ���������, 1 - ���������� � ���������), 5 ������ - ����� ������ (0 - �������� ��������� ���������, 1 - ���) 
//��������� ���������� �� ������
//�������� mode: 1 ������ - ������������ ����� (0 - ���, 1 - ����)
//2 ������ - ����������� DCM (0 - �� �����, 1 - �����)
//3 ������ - ������������� (0 - ���, 1 - ����)
//4 ������ - ����������� �������� ����� (0 - ���������, 1 - ���������� � ���������)
//5 ������ - ����� ������ (0 - �������� ��������� ���������, 1 - ���)
       //UO(MPins, Upwr, Ugnd, DRL, DRH, DRL1, DRH1, ILimDCM, UDCM, LBound, UBound, StartFK, StopFK, TestName, Mode);
Function UO(Var MPins : TLargeSet; Upwr, Ugnd, DRL, DRH, DRL1, DRH1, ILimDCM, UDCM, LBound, UBound : Real; StartFK : Integer;
	     Var  StopFK : TVectors; TestName : String; Mode : Integer) : Real;
  Var
    Corr, Upwr_real, U : Real;
  Begin
    
    //��� ������������� ������� ��� ������ � ������ ������ �����
    if not VEEON then
      Ugnd := 0.0;
    Upwr	:= Upwr + Ugnd;
    UDCM	:= UDCM + Ugnd;
    DRL		:= DRL  + Ugnd;
    DRH		:= DRH  + Ugnd;
    DRL1	:= DRL1 + Ugnd;
    DRH1	:= DRH1 + Ugnd;
    Corr	:= NULL;

    //������������� ����������� ������� ���������� � ���� �� DCM � ��������� ��� 
    SetDCM(UDCM, RAuto, IConnect, DCM_1);
    DelayMS(TimeUSB);

    ConnectDCM(DCM_Off);
    DelayMS(TimeUSB);

    //������� ������������ ������
    if (mode and !0001) = 0 then
      begin
      //������ ������� ������ ���������� �� ���������� � ���������
      //UpDown(   Boards, Upwr, Ugnd,  DRH,  DRL, DRH1, DRL1, dU);
	UpDown(BoardList, NULL, NULL, NULL, NULL, NULL, NULL, dU);
	
	//��������� ��� ����
	ConnectPins([1..256], []);
	DelayMS(TimeUSB);
	
	//������������� 0 �� ���� ������� ���� ����
	for K := 0 to 15 do
	  begin
	    SendPat(K, 0, 0);
	    DelayMS(TimeUSB);
	  end;
	  
	//���������� ������� ���� � ���������
	ConnectPins(InPins, [Pin_To_Drivers]);
	DelayMS(TimeUSB);
	
	//��� ������������� ���������� �������� ���� � ���������
        if (mode and !1000) <> 0 then
          begin
            ConnectPins(OutPins, [Pin_To_Drivers]);
	    DelayMS(TimeUSB);
          end;
      end;


  //������ ������ ������ ����������
  //UpDown(   Boards, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
    UpDown(BoardList, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
  
    //��� ������������� ��������� ����� �������� ����������� �� �������
    if VEEON then
      begin
	//���������� DCM � ���������, ������������ �����
	ConnectDCM(ComDCMGND);
	DelayMS(TimeMeasU);
	
	//�������� �������� �������� ��������� ����������
	Corr := -1.0 * OnlyMeasDCMVoltage(ADCRange_AUTO);

	//��������� DCM
	ConnectDCM(DCM_Off);
	DelayMS(TimeUSB);
      end;
    
    //��� ������������� ������������� �������� ����������� ��������� �������
    if (mode and !0100) <> 0 then
      begin
        //���������� DCM � ���������
        ConnectDCM(ComDCMPWR);
	DelayMS(TimeMeasU);

	//������������� ������ ������ ���������� � ���� �� DCM
        if Upwr > 7.5 then
          SetDCM(UDCM, R20_42, ILimDCM, DCM_Auto);
        else
          SetDCM(UDCM, R8, ILimDCM, DCM_Auto);
        DelayMs(TimeMeasU);

	//�������� �������� ���������� �� ���������
	Upwr_real := (OnlyMeasDCMVoltage(ADCRange_AUTO) + OnlyMeasDCMVoltage(ADCRange_AUTO) + OnlyMeasDCMVoltage(ADCRange_AUTO))/3;
	InfoBox(Upwr_real,'');
	//������� ����������� � �������� ��������� �������
	Upwr_real := Upwr - Upwr_real;
	//��������� ���������� ����������� � ���������
	Corr := Corr + Upwr_real;

	//��������� DCM
	ConnectDCM(DCM_Off);
	DelayMS(TimeUSB);
      end;

    //�������� ������������� ���������� DCM ����� ����������� �����
    if (mode and !0010) = 0 then
      begin
        ConnectDCM(DCM_To_Pins);
        DelayMS(TimeUSB);
        if not ILimDCM > 1.0e-3 then
        begin
          SetDCM(UDCM, RAuto, ILimDCM, DCM_Auto);
          DelayMS(TimeUSB);
        end;
      end;

    //���������� ���������� ���������� �� ������ ������ ��
    First_Run:= True;
    Last_Run := 0;

    //���� �� ���� ������� ��
    for K := FirstPin to LastPin do
      begin
        //���������� ����� ���� �� �� � ������ ����������
        if not(K in MPins) then
	  continue;
   
        //���� ��� ������ ������ ��� ����� ������� �������� �� ��������� � ���������� �������� �������� ��������� ��
        if StopFK[Last_Run]<>StopFK[K - FirstPIn] or First_Run then
          begin
            start_beg_hand(StartFK, StopFK[K - FirstPIn], 1, 0, 1);
            repeat intStop := check_stop(intTact);
            DelayCompMks(100);
            until intStop <> 0;
            First_Run := False;
          end;
      
        //��������� ����� ���������� ������������ ������
        Last_Run := K - FirstPIn;
      
        //���������� ������� ����� � ����������
        ConnectPin(K, [Pin_To_DCM]);
        DelayMS(TimeUSB);

        //��� ������������� ���������� DCM � ������������� ����
   
        if (mode and !0010) <> 0 then
          begin
	    ConnectDCM(DCM_To_Pins);
	    DelayMS(TimeUSB);
	  end;
      
        //������������� ������ ������ ���������� � ���� �� DCM
        if Upwr > 7.5 then
          SetDCM(UDCM, R20_42, ILimDCM, DCM_Auto);
        else
          SetDCM(UDCM, R8, ILimDCM, DCM_Auto);
        DelayMs(TimeMeasU);
      
        //���������� ��������� ����������
        U := OnlyMeasDCMVoltage(ADCRange_AUTO) + Corr;
        DelayMs(TimeUSB);

        //��� ������������� �������� ��� DCM ����� �����������
        if ILimDCM > 1.0e-3 then
          begin
	    SetDCM(UDCM, RAuto, Iconnect, DCM_1);
	    DelayMs(TimeUSB);
	  end;

        //���� ����� ��������������� ��� ������ ����� ����������� �������� �����
        //���������� ����� � ���������
        if (K in InPins) then 
	  ConnectPin(K, [Pin_To_Drivers]);
	else
          begin
	    if (mode and !1000) <> 0 then
	      ConnectPin(K, [Pin_To_Drivers]);
	    else
              //� ��������� ������ ��������� �� � ������ 
	      ConnectPin(K, []);
	  end;
        DelayMS(TimeUSB);

        //��� ������������� ��������� DCM �� ������������� ����
        if (mode and !0010) <> 0 then
          begin
            ConnectDCM(DCM_Off);
	    DelayMS(TimeUSB);
          end;

        //���� �� ����� ����� "�� ������" ������� ���������� ���������
        if (mode and !10000) = 0 then
        //PrintResTU(TestName,       PinNum,      Upwr, Value, LBound, UBound, Dim);
          PrintResTU(TestName, K - FirstPin, Upwr-Ugnd,     U, LBound, UBound, Uno);

      end;
    
    //��������� DCM �� ������������� ����
    ConnectDCM(DCM_Off);
    DelayMS(TimeUSB);

    //���������� �� ������� ���������� ��������
    result := U;

  End;
//<Uo>


//<Ili>
//15
//Function ILI(Var MeasPin : TLargeSet; Upwr, Ugnd, UDCM, DRL, DRH, DRL1, DRH1, LBound, UBound, Dim : Real; StartFK : Integer; Var  StopFK : TVectors; TestName : String; mode : Integer) : Real;
//InPins; ������ ����������� �������
//5.0; ���������� �������, �
//0.0; �������� �����, �
//5.0; �������� ���������� ����������, �
//0.4; ���������� ������� ������, �
//4.5; ���������� �������� ������, �
//0.0; ���.���������� ������� ������, �
//0.0; ���.���������� �������� ������, �
//-12.0e-9; ������ �������, �
//1.0e-6; ������� �������, �
//micro; �������� (Uno: e-0, milli: e-3, micro: e-6, nano: e-9, pico: e-12)
//0000; ��������� ������ FK
//VectorIIL; �������� ������ FK
//3.���.����.��.IIL,���?3. ��� ������� ������ IIL, ���; ��� �����
//0000; ��������� ������ (1 ������ - ������������ ����� (0 - ���, 1 - ����), 2 ������ - ����������� DCM (0 - �� �����, 1 - �����), 3 ������ - �� ������������, 4 ������ - ����������� �������� ����� (0 - ���������, 1 - ���������� � ���������), 5 ������ - ����� ������ (0 - �������� ��������� ���������, 1 - ���) 
//��������� ���� ������
//�������� mode: 1 ������ - ������������ ����� (0 - ���, 1 - ����)
//2 ������ - ����������� DCM (0 - �� �����, 1 - �����)
//3 ������ - �� ������������
//4 ������ - ����������� �������� ����� (0 - ���������, 1 - ���������� � ���������)
//5 ������ - ����� ������ (0 - �������� ��������� ���������, 1 - ���)
//       ILI( MeasPin, Upwr, Ugnd, UDCM, DRL, DRH, DRL1, DRH1, LBound, UBound, Dim, StartFK, StopFK, TestName, mode);
Function ILI(Var MeasPin : TLargeSet; Upwr, Ugnd, UDCM, DRL, DRH, DRL1, DRH1, LBound, UBound, Dim : Real; StartFK : Integer;
              Var  StopFK : TVectors; TestName : String; mode : Integer) : Real;
  Var
    I : Real;
    IDCM : Real;
  Begin
    //������� ������������ ������� ��� ����������� ����������� �� ���� ���������-����������
    if ABS(LBound) > ABS(UBound) then 
      IDCM := ABS(LBound) 
    else 
      IDCM := ABS(UBound);
    
    //������������� �������� ������� � ������� � 50% ��� 1.79 ��� ��� ����� �����
    if IDCM < 1.5e-6 then 
      IDCM := 1.79e-6 
    else 
      IDCM := IDCM * 1.5;

    //����������, ��� ��� ����������� ������ ��������� ����� �������� ����� 0
    if VEEON <> true then 
      Ugnd := 0.0;
    
    //������������ �������� ���������� � ������ ��������� �����
    Upwr := Upwr + Ugnd;
    DRL  := DRL  + Ugnd;
    DRH  := DRH  + Ugnd;
    DRL1 := DRL1 + Ugnd;
    DRH1 := DRH1 + Ugnd;
    UDCM := UDCM + Ugnd;
   
    if (mode and !0001) = 0 then
    //������ ������� ������ ���������� �� ���������� � ���������
    //UpDown(   Boards, Upwr, Ugnd,  DRH,  DRL, DRH1, DRL1, dU);
      UpDown(BoardList, NULL, NULL, NULL, NULL, NULL, NULL, dU);

    //���������� � ��������� ������� ����
    ConnectPins(InPins, [Pin_To_Drivers]);
    DelayMS(TimeUSB);

    //��� ������������� ���������� �������� ���� � ���������
    if (mode and !1000) <> 0 then
      begin
        ConnectPins(OutPins, [Pin_To_Drivers]);
	DelayMS(TimeUSB);
      end;

    //��� ������������� ��������� ��� DCM �� ������ ����������
    if IDCM > 1.0e-3 then
      SetDCM(UDCM, RAuto, IConnect, DCM_1);
    else
      SetDCM(UDCM, RAuto, IDCM, DCM_Auto);
    DelayMS(TimeUSB);

    //���������� DCM � ������������� ����, ���� �� ������� ��������
    if (mode and !0010) = 0 then
      ConnectDCM(DCM_To_Pins);
    else
      ConnectDCM(DCM_Off);
    DelayMS(TimeUSB);

  //������������� ������ ������ �� ���������� � ���������
  //UpDown(   Boards, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
    UpDown(BoardList, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);

    //���������� ���������� ����������
    First_Run := true;
    Last_Run := 0;

    //���� �� ���� �����
    for K := FirstPin to LastPin do
      begin
        //���������� �������� ���� ����� �� � ������ �����������
        if not(K in MeasPin) then continue;
        
	//�������� ������������� ���������� ������� ��
	if StopFK[Last_Run] <> StopFK[K - FirstPin] or First_Run then
          begin
       	    start_beg_hand(StartFK, StopFK[K - FirstPin], 1, 0, 1);
            repeat intStop := check_stop(intTact); DelayCompMks(100);
      	    until intStop <> 0;
      	    First_Run := false;
          end;
	
	//���������� ����� ���������� ������������ ������
        Last_Run := K - FirstPin;

        //���������� ����� � ������������� ����
        ConnectPin(K, [Pin_To_DCM]);
	DelayMs(TimeUSB);

	//��� ������������� ���������� � ��� �� DCM
	if (mode and !0010) <> 0 then
	  begin
	    ConnectDCM(DCM_To_Pins);
	    DelayMS(TimeUSB);
	  end;
	
	//��� ������������� ����������� ��� DCM
	if IDCM > 1.0e-3 then
	  begin
	    SetDCM(UDCM, RAuto, IDCM, DCM_Auto);
	    DelayMS(TimeUSB);
	  end;
        DelayMs(TimeMeasI);

	//�������� �������� ����
	I := OnlyMeasDCMCurent;
        DelayMs(TimeUSB);
	
	//��� ������������� ��������� ��� DCM ��� �������������
	if(IDCM > 1.0e-3) then
	  begin
            SetDCM(UDCM, RAuto, Iconnect, DCM_Auto);
            DelayMs(TimeUSB);
	  end;

        //��� ������������� ���������� ����� � ��������� ��� ��������� � ������
	if (K in InPins) then 
	  ConnectPin(K, [Pin_To_Drivers]);
	else
          begin
	    if (mode and !1000) <> 0 then
	      ConnectPin(K, [Pin_To_Drivers]);
	    else 
	      ConnectPin(K, []);
	  end;
	  
	//���� �� ������ ����� "�� ������" ������� ���������� ���������
	if (mode and !10000) = 0 then
        //PrintResTU(TestName,       PinNum,      Upwr, Value, LBound, UBound, Dim);
          PrintResTU(TestName, K - FirstPin, Upwr-Ugnd,     I, LBound, UBound, Dim);  
      end;

    //��������� DCM
    ConnectDCM(DCM_Off);
    DelayMS(TimeUSB);

    //���������� ���������� ��������
    result := I;

  End;
//<Ili>


//<Icc>
//12
//Procedure ICC(Upwr, Ugnd, DRL, DRH, DRL1, DRH1, LBound, UBound, Dim : Real; StartFK, StopFK : Integer; TestName : String);
//5.0; ���������� �������, �
//0.0; �������� �����, �
//0.4; ���������� ������� ������, �
//4.5; ���������� �������� ������, �
//0.0; ���.���������� ������� ������, �
//0.0; ���.���������� �������� ������, �
//-1.5e-6; ������ �������, �
//1.3e-3; ������� �������, �
//milli; �������� (Uno: e-0, milli: e-3, micro: e-6, nano: e-9, pico: e-12)
//0000; ��������� ������ FK
//0000; �������� ������ FK
//6.���.�������.ICC,��?6. ��� ����������� ICC, ��; ��� �����
//��������� ���� �����������

//������������ �� DCM ��� 0�
Procedure ICC_ON;
  Begin
  //������ ������� ���������� �� ���������� � ���������
  //UpDown(   Boards, Upwr, Ugnd,  DRH,  DRL, DRH1, DRL1, dU);
    UpDown(BoardList, NULL, NULL, NULL, NULL, NULL, NULL, dU);
    
    //������ ������� ���������� � ����������� ��� �� DCM
    SetDCM(NULL, R8, Iconnect, DCM_1);
    DelayMs(TimeUSB);

    //���������� DCM ������ ��������� ����������
    ConnectDCM(ComDCMPwr);
    DelayMs(TimeUSB);

    //������������� ����������� DCM �� ���� ������ ����������� ����������
    SetDCM(NULL, R8, IlimPwr, DCM_Auto);
    DelayMs(TimeUSB);

    //��������� �������� ����������
    ConnectPower(PWR, Source_Off);
    DelayMs(TimeUSB);

    //��������� �������� ��������� ��� ������������ �����������
    PWROLD := PWR;
    PWR	   := pDCM;

    //��������� �������� ������
    ConnectPins(OutPins, []);
    DelayMs(TimeUSB);

    //���������� ������� ������ � ���������
    ConnectPins(InPins, [Pin_To_Drivers]);
    DelayMs(TimeUSB);
  End;

//��������� ���� �����������
	//ICC(Upwr, Ugnd, DRL, DRH, DRL1, DRH1, LBound, UBound, Dim, StartFK, StopFK, TestName);
Procedure ICC(Upwr, Ugnd, DRL, DRH, DRL1, DRH1, LBound, UBound, Dim : Real; StartFK, StopFK : Integer; TestName : String);
  Var
    IDCM, I : Real;
  Begin
    //���������� ������������ ������� ��� ��������� ����������� �� ����
    if ABS(LBound) > ABS(UBound) then
      IDCM := ABS(LBound);
    else
      IDCM := ABS(UBound);

    //������������� ����������� �� ���� �� 50% ������ ������� ��� 1.79 ��� ��� ����� �����
    if IDCM < 1.5e-6 then
      IDCM := 1.79e-6
    else
      IDCM := IDCM * 1.5;
    
    //����������, ��� ��� ����������� ������ ��������� ����� ��������� ����� 0
    if VEEON <> true then
      Ugnd := 0.0;

    //������������ ���������� � ������ �������� �����
    Upwr := Upwr + Ugnd;
    DRL  := DRL  + Ugnd;
    DRH  := DRH  + Ugnd;
    DRL1 := DRL1 + Ugnd;
    DRH1 := DRH1 + Ugnd;
    
  //������������� ������ ������ ����������
  //UpDown(   Boards, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
    UpDown(BoardList, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);

    SetDCM(Upwr, RAuto, ILimPwr, DCM_Auto);
    DelayMS(TimeUSB);
   
    //��������� ��
    start_beg_hand(StartFK, StopFK, 1, 0, 1);
    DelayMS(TimeMeasI*2);

    //��������� ��� DCM ��� �������� �����������
    SetDCM(Upwr, RAuto, IDCM, DCM_Auto);
    DelayMS(TimeMeasI*2);

    //�������� ��� �����������
    I := (OnlyMeasDCMCurent + OnlyMeasDCMCurent + OnlyMeasDCMCurent) / 3;
    DelayMS(TimeUSB);
    
    //�������������� ��������� �� ��� ��������� ������������� ����
    StopPP;

    //����������� ��� DCM ��� ����������� ������
    SetDCM(Upwr, RAuto, ILimPwr, DCM_Auto);
    DelayMS(TimeUSB);

  //����� ����������� ����������
  //PrintResTU(TestName,             PinNum,      Upwr, V, LBound, UBound, Dim);
    PrintResTU(TestName, LastPin - FirstPin, Upwr-Ugnd, I, LBound, UBound, Dim);
  End;

//������������ �� �������� ��� 0�
Procedure ICC_OFF;
  Begin
  //UpDown(   Boards, Upwr, Ugnd,  DRH,  DRL, DRH1, DRL1,   dU);
    UpDown(BoardList, NULL, NULL, NULL, NULL, NULL, NULL, 0.25);

    //���������� ����������� ����� �������� ���������
    PWR := PWROLD;
    
    //�������� �������� �������))
    SetPower(PWR, NULL, ILimPwr, Source_On);
    DelayMS(TimeUSB);

    //������ ������� ���������� � ����������� ��� �� DCM
    SetDCM(NULL, R8, IConnect, DCM_1);
    DelayMS(TimeUSB);

    //��������� DCM
    ConnectDCM(DCM_Off);
    DelayMS(TimeUSB);
  End;
//<Icc>


//<Fk>
//11
//Procedure FK(Upwr, Ugnd, DRL, DRH, DRL1, DRH1, CPL, CPH : Real; StartFK, StopFK : Integer; TestName: String);
//5.0; ���������� �������, �
//0.0; �������� �����, �
//0.4; ���������� ������� ������, �
//4.5; ���������� �������� ������, �
//0.0; ���.���������� ������� ������, �
//0.0; ���.���������� �������� ������, �
//0.8; ������ ������� ��������������, �
//4.0; ������� ������� ��������������, �
//0000; ��������� ������ FK
//0000; �������� ������ FK
//7.��������.�����.FK,B?7. �������������� �������� FK, �; ��� �����
//���������� ������������� ��
//        FK(Upwr, Ugnd, DRL, DRH, DRL1, DRH1, CPL, CPH, StartFK, StopFK, TestName)
Procedure FK(Upwr, Ugnd, DRL, DRH, DRL1, DRH1, CPL, CPH : Real; StartFK, StopFK : Integer; TestName: String);
  Var
    FKState, StopVector	: Integer;
  Begin
  
    //�������� ��������� ��� ����������� �����
    if VEEON <> true then
      Ugnd := 0.0;
    
    //�������� ���������� � ������ �������� �����
    Upwr := Upwr + Ugnd;
    DRL  := DRL  + Ugnd;
    DRH  := DRH  + Ugnd;
    CPL  := CPL  + Ugnd;
    CPH  := CPH  + Ugnd;
    DRL1 := DRL1 + Ugnd;
    DRH1 := DRH1 + Ugnd;

  //������ ������ ���������� �� ��������� � ����������
  //UpDown(   Boards, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
    UpDown(BoardList, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
    
    //������ ������ ������������
    LoadDACPinsLevels(BoardList, DRH, DRL, CPH, CPL, DRH1, DRL1);
    
    //���������� ��� ���� � ���������
    ConnectPins(AllPins, [Pin_To_Drivers]);
    DelayMS(TimeUSB);

    //��������� �� � �������� ��� �����
    start_beg_hand(StartFK, StopFK, 1, 1, 1);
    repeat FKState := check_stop(StopVector); DelayCompMks(100);
    until  FKState <> 0;
    FKState := check_stop(StopVector);
    StopPP;
    
  //������� ��������� ���������� ��
  //PrintResTU(TestName, PinNum,       Upwr,          V, LBound, UBound, Dim);
    PrintResTU(TestName,     -1,  Upwr-Ugnd, StopVector, StopFK, StopFK, Uno); 

    //��������� ��� ������
    ConnectPins(AllPins, []);
    DelayMS(TimeUSB);

  End;
//<Fk>


//<Tp>
//15
//Procedure Tp(Var MeasPin : TLargeSet; Upwr, Ugnd, DRL, DRH, DRL1, DRH1, CPL, CPH, TinT, UBound : Real; StartFK, StopFK: Integer; TestName: String; SFPMode : Boolean);
//OutPins; ������ ����������� �������
//5.0; ���������� �������, �
//0.0; �������� �����, �
//0.4; ���������� ������� ������, �
//4.5; ���������� �������� ������, �
//0.0; ���.���������� ������� ������, �
//0.0; ���.���������� �������� ������, �
//2.20; ������ ������� ��������������, �
//2.30; ������� ������� ��������������, �
//Tin; �������� ���������������, ���������� ��� �����, ��
//15; ������� �������, ��
//0001; ��������� ������ FK
//0006; �������� ������ FK
//8.��.���.�����.TpHL,��?8. ����� �������� ��������������� TpHL, ��; ��� �����
//true; ����� ����� ������� ��������� � ������ ��� ����� �������
//��������� ������� ��������
	//Tp(MeasPin,  Upwr, Ugnd, DRL, DRH, DRL1, DRH1, CPL, CPH,  TinT, UBound,  StartFK, StopFK,  TestName, SFPMode);
Procedure Tp(Var MeasPin : TLargeSet; Upwr, Ugnd, DRL, DRH, DRL1, DRH1, CPL, CPH, TinT, UBound : Real; StartFK,
		   StopFK: Integer; TestName: String; SFPMode : Boolean);
  Var
    Tp : Real;
    //���������� ������ ��� �������� � ���������� �������
    //������������ � ��� ������������ ����� ������� � ���������
    V : Integer;
  Begin
    //��������� DCM
    ConnectDCM(DCM_Off);
    DelayMS(TimeUSB);
    
    //�������� ��������� ��� ����������� �����
    if VEEON <> true then
      Ugnd := 0.0;
    
    //������������ ���������� � ������ �������� �����
    Upwr := Upwr + Ugnd;
    DRL  := DRL  + Ugnd;
    DRH  := DRH  + Ugnd;
    CPL  := CPL  + Ugnd;
    CPH  := CPH  + Ugnd;
    DRL1 := DRL1 + Ugnd;
    DRH1 := DRH1 + Ugnd;
    
  //������������� ���������� �� ��������� � ����������
  //UpDown(   Boards, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
    UpDown(BoardList, Upwr, Ugnd, DRH, DRL, DRH1, DRL1, dU);
    
    //������������� ������ ������������
    LoadDACPinsLevels(BoardList, DRH, DRL, CPH, CPL, DRH1, DRL1);
    DelayMS(TimeUSB);
    
    //���������� ��� ���� � ���������
    ConnectPins(AllPins, [Pin_To_Drivers]);
    DelayMS(TimeUSB);
    
    //���� �� ���� �������
    for K := FirstPin to LastPin do
      begin
        //���������� �������� ���� ����� �� � ������ �����������
        if not(K in MeasPin) then continue;

	//����� ������ ��������
	if SFPMode then
	  Tp := SFP(StartFK, StopFK, K, V);
	else
	  Tp := SRFP(StartFK, StopFK, K, V);

	Tp := Tp - TinT;
	  
	//��� ������������� ��������� ������ ������� � ������ ��������
	if (Tp < 0)  then
          begin
            if Tp + TinT > 0 then
            //PrintResTU(TestName,       PinNum,      Upwr,  V, LBound, UBound, Dim);
	      PrintResTU(TestName, K - FirstPin, Upwr-Ugnd, Tp,   -TinT, UBound, Uno);
  	    else
	    //PrintResTU(TestName,       PinNum,      Upwr,  V, LBound, UBound, Dim);
	      PrintResTU(TestName, K - FirstPin, Upwr-Ugnd, Tp,   NULL, UBound, Uno);
	  end
	else
        //PrintResTU(TestName,       PinNum,      Upwr,  V, LBound, UBound, Dim);
	  PrintResTU(TestName, K - FirstPin, Upwr-Ugnd, Tp,   NULL, UBound, Uno);
      end;

    //��������� ��� ������
    ConnectPins(AllPins, []);
    DelayMS(TimeUSB);

  End;
//<Tp>


//<Endofsource>
