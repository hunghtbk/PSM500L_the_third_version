#include "qextserialport.h"
#include "qextserialenumerator.h"
#include "dialog.h"
#include "ui_dialog.h"
#include <QtCore>
#include "function_Check.h"
#include <QPixmap>
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    //! [0]
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->portBox1->addItem(info.portName);
    //make sure user can input their own port name!
    ui->portBox1->setEditable(true);

    ui->baudRateBox1->addItem("1200", BAUD1200);
    ui->baudRateBox1->addItem("2400", BAUD2400);
    ui->baudRateBox1->addItem("4800", BAUD4800);
    ui->baudRateBox1->addItem("9600", BAUD9600);
    ui->baudRateBox1->addItem("19200", BAUD19200);
    ui->baudRateBox1->addItem("38400", BAUD38400);
    ui->baudRateBox1->setCurrentIndex(3);

    ui->parityBox1->addItem("NONE", PAR_NONE);
    ui->parityBox1->addItem("ODD", PAR_ODD);
    ui->parityBox1->addItem("EVEN", PAR_EVEN);

    ui->dataBitsBox1->addItem("5", DATA_5);
    ui->dataBitsBox1->addItem("6", DATA_6);
    ui->dataBitsBox1->addItem("7", DATA_7);
    ui->dataBitsBox1->addItem("8", DATA_8);
    ui->dataBitsBox1->setCurrentIndex(3);

    ui->stopBitsBox1->addItem("1", STOP_1);
    ui->stopBitsBox1->addItem("2", STOP_2);

    ui->queryModeBox1->addItem("Polling", QextSerialPort::Polling);
    ui->queryModeBox1->addItem("EventDriven", QextSerialPort::EventDriven);
    //! [0]

    ui->led1->turnOff();

    timer = new QTimer(this);
    timer->setInterval(40);
    //! [1]
    PortSettings settings = {BAUD9600, DATA_8, PAR_NONE, STOP_1, FLOW_OFF, 10};
    port = new QextSerialPort(ui->portBox1->currentText(), settings, QextSerialPort::Polling);
    //! [1]

    enumerator = new QextSerialEnumerator(this);
    enumerator->setUpNotifications();

    connect(ui->baudRateBox1, SIGNAL(currentIndexChanged(int)), SLOT(onBaudRateChanged(int)));
    //connect(ui->cmbModulation1, SIGNAL(currentIndexChanged(int)), SLOT(onModulation1(int)));
    //connect(ui->cmbModulation2, SIGNAL(currentIndexChanged(int)), SLOT(onModulation2(int)));
    connect(ui->parityBox1, SIGNAL(currentIndexChanged(int)), SLOT(onParityChanged(int)));
    connect(ui->dataBitsBox1, SIGNAL(currentIndexChanged(int)), SLOT(onDataBitsChanged(int)));
    connect(ui->stopBitsBox1, SIGNAL(currentIndexChanged(int)), SLOT(onStopBitsChanged(int)));
    connect(ui->queryModeBox1, SIGNAL(currentIndexChanged(int)), SLOT(onQueryModeChanged(int)));
    connect(ui->timeoutBox1, SIGNAL(valueChanged(int)), SLOT(onTimeoutChanged(int)));
    connect(ui->portBox1, SIGNAL(editTextChanged(QString)), SLOT(onPortNameChanged(QString)));
    connect(ui->openCloseButton1, SIGNAL(clicked()), SLOT(onOpenCloseButtonClicked()));
    connect(ui->sendButton1, SIGNAL(clicked()), SLOT(onSendButtonClicked()));
    connect(timer, SIGNAL(timeout()), SLOT(onReadyRead()));
    connect(port, SIGNAL(readyRead()), SLOT(onReadyRead()));

    connect(enumerator, SIGNAL(deviceDiscovered(QextPortInfo)), SLOT(onPortAddedOrRemoved()));
    connect(enumerator, SIGNAL(deviceRemoved(QextPortInfo)), SLOT(onPortAddedOrRemoved()));

    setWindowTitle(tr("Embedded Networking Research Group Lab 411"));
    ui->lineEdit_Mod_n->setReadOnly(true);
    ui->lineEdit_Mod_k->setReadOnly(true);


    ui->lineEdit_DeMod_n->setReadOnly(true);
    ui->lineEdit_DeMod_k->setReadOnly(true);



    ui->lineEdit1->setText("root");

}

Dialog::~Dialog()
{
    delete ui;
    delete port;
}

void Dialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void Dialog::onPortNameChanged(const QString & /*name*/)
{
    if (port->isOpen()) {
        port->close();
        ui->led1->turnOff();
    }
}
//! [2]
void Dialog::onBaudRateChanged(int idx)
{
    port->setBaudRate((BaudRateType)ui->baudRateBox1->itemData(idx).toInt());
}

void Dialog::onParityChanged(int idx)
{
    port->setParity((ParityType)ui->parityBox1->itemData(idx).toInt());
}

void Dialog::onDataBitsChanged(int idx)
{
    port->setDataBits((DataBitsType)ui->dataBitsBox1->itemData(idx).toInt());
}

void Dialog::onStopBitsChanged(int idx)
{
    port->setStopBits((StopBitsType)ui->stopBitsBox1->itemData(idx).toInt());
}

void Dialog::onQueryModeChanged(int idx)
{
    port->setQueryMode((QextSerialPort::QueryMode)ui->queryModeBox1->itemData(idx).toInt());
}

void Dialog::onTimeoutChanged(int val)
{
    port->setTimeout(val);
}
//! [2]
//! [3]
void Dialog::onOpenCloseButtonClicked()
{
    if (!port->isOpen()) {
        port->setPortName(ui->portBox1->currentText());
        port->open(QIODevice::ReadWrite);
    }
    else {
        port->close();
    }

    //If using polling mode, we need a QTimer
    if (port->isOpen() && port->queryMode() == QextSerialPort::Polling)
        timer->start();
    else
        timer->stop();

    //update led's status
    ui->led1->turnOn(port->isOpen());
}
//! [3]
//! [4]
void Dialog::onSendButtonClicked()
{

    if (port->isOpen() && !ui->sendEdit1->toPlainText().isEmpty())
        port->write(ui->sendEdit1->toPlainText().toLatin1());
        ui->sendEdit1->clear();
}

void Dialog::onReadyRead()
{
    if (port->bytesAvailable()) {
        ui->recvEdit1->moveCursor(QTextCursor::End);
        ui->recvEdit1->insertPlainText(QString::fromLatin1(port->readAll()));



    }
}

void Dialog::onModulation1(int idx)
{

}

void Dialog::onModulation2(int idx)
{

}

void Dialog::onPortAddedOrRemoved()
{
    QString current = ui->portBox1->currentText();

    ui->portBox1->blockSignals(true);
    ui->portBox1->clear();
    foreach (QextPortInfo info, QextSerialEnumerator::getPorts())
        ui->portBox1->addItem(info.portName);

    ui->portBox1->setCurrentIndex(ui->portBox1->findText(current));

    ui->portBox1->blockSignals(false);
}

//! [4]

void Dialog::on_pushButton_clicked()
{
    qApp->quit();
}



void Dialog::on_pushButton_2_clicked()
{
    QString a=ui->lineEdit1->text();a=a+"\n";

    qDebug()<<a;

    QByteArray ba = a.toLatin1();
    const char *c_str2 = ba.data();
    port->write(c_str2);
    ui->label_name->setText("PassWord");
    ui->lineEdit1->setText("datum");
}

void Dialog::on_pushButton_5_clicked()
{
    qApp->quit();
}

void Dialog::on_comboBox_Mod_FECT_currentIndexChanged(const QString &arg1)
{
    QString a=ui->comboBox_Mod_FECT->currentText();

    if(a=="UnCoded"){
        while(ui->comboBox_Mod_FECO->count()>0){
            ui->comboBox_Mod_FECO->removeItem(0);
        }
    ui->comboBox_Mod_FECO->addItem("None");
   }
    else if(a=="Viterbi"){
        while(ui->comboBox_Mod_FECO->count()>0){
            ui->comboBox_Mod_FECO->removeItem(0);
        }
        ui->comboBox_Mod_FECO->addItem("Normal");
        ui->comboBox_Mod_FECO->addItem("Swap C0/C1");
        ui->comboBox_Mod_FECO->addItem("CT");
    }
    else if(a=="TCM"){
        while(ui->comboBox_Mod_FECO->count()>0){
            ui->comboBox_Mod_FECO->removeItem(0);
        }

        ui->comboBox_Mod_FECO->addItem("Normal");
    }
    else if(a=="TPC"){
        while(ui->comboBox_Mod_FECO->count()>0){
            ui->comboBox_Mod_FECO->removeItem(0);
        }

        ui->comboBox_Mod_FECO->addItem("Advanced");
        ui->comboBox_Mod_FECO->addItem("M5 Full");
        ui->comboBox_Mod_FECO->addItem("M5 Short");
        ui->comboBox_Mod_FECO->addItem("M5 Legacy");
        ui->comboBox_Mod_FECO->addItem("CT");
    }
    else if(a=="LDPC"){
        while(ui->comboBox_Mod_FECO->count()>0){
            ui->comboBox_Mod_FECO->removeItem(0);
        }

        ui->comboBox_Mod_FECO->addItem("256 Block");
        ui->comboBox_Mod_FECO->addItem("512 Block");
        ui->comboBox_Mod_FECO->addItem("1K Block");
        ui->comboBox_Mod_FECO->addItem("2K Block");
        ui->comboBox_Mod_FECO->addItem("4K Block");
        ui->comboBox_Mod_FECO->addItem("8K Block");
        ui->comboBox_Mod_FECO->addItem("16K Block");

    }
    else if(a=="S-Tec"){
        while(ui->comboBox_Mod_FECO->count()>0){
            ui->comboBox_Mod_FECO->removeItem(0);
        }

        ui->comboBox_Mod_FECO->addItem("Large");
        ui->comboBox_Mod_FECO->addItem("Medium");
        ui->comboBox_Mod_FECO->addItem("Small");
        ui->comboBox_Mod_FECO->addItem("XLarge");
    }
    qDebug()<<a;
}

void Dialog::on_pushButton_3_clicked()
{
    //Set Mod
   /*Set Data Rate*/
    QString LENH1;
    QString _cmd1=ui->lineEdit_Mod_DR->text();
    LENH1="mq mdr "+_cmd1+"\n";
    QByteArray ba1 = LENH1.toLatin1();
    const char *c_str1 = ba1.data();
    port->write(c_str1);

    /*Set Modulation Mode*/
    QString LENH2;
    int _intcmd=CheckcmbModulation_1(ui->comboBox_Mod_MM->currentText());
    switch(_intcmd){
    case 0: LENH2="m500ctl 0 mdm 0\n";break;
    case 1: LENH2="m500ctl 0 mdm 1\n";break;
    case 2: LENH2="m500ctl 0 mdm 2\n";break;
    case 3: LENH2="m500ctl 0 mdm 3\n";break;
    case 4: LENH2="m500ctl 0 mdm 4\n";break;
    case 6: LENH2="m500ctl 0 mdm 6\n";break;
    default: break;
    }
    QByteArray ba2=LENH2.toLatin1();
    const char *c_str2=ba2.data();
    port->write(c_str2);

    /*Set FEC Type*/

    QString LENH3;

    int _cmd3=Check_FEC_Type(ui->comboBox_Mod_FECT->currentText());
    switch(_cmd3){
        case 0: LENH3="m500ctl 0 mdf 0\n";break;
        case 1: LENH3="m500ctl 0 mdf 1\n";break;
        case 2: LENH3="m500ctl 0 mdf 2\n";break;
        case 4: LENH3="m500ctl 0 mdf 4\n";break;
        case 5: LENH3="m500ctl 0 mdf 5\n";break;
        case 6: LENH3="m500ctl 0 mdf 6\n";break;
    }
    QByteArray ba3=LENH3.toLatin1();
    const char *c_str3=ba3.data();
    port->write(c_str3);

    /*set FEC Option*/

    QString LENH4;

    int _cmd4=Check_FEC_Option(ui->comboBox_Mod_FECO->currentText());
    switch(_cmd4){
        case 0: LENH4="m500ctl 0 mdo 0\n";break;
        case 1: LENH4="m500ctl 0 mdo 1\n";break;
        case 2: LENH4="m500ctl 0 mdo 2\n";break;
        case 3: LENH4="m500ctl 0 mdo 3\n";break;
        case 4: LENH4="m500ctl 0 mdo 4\n";break;
        case 5: LENH4="m500ctl 0 mdo 5\n";break;
        case 6: LENH4="m500ctl 0 mdo 6\n";break;
    }
    QByteArray ba4=LENH4.toLatin1();
    const char *c_str4=ba4.data();
    port->write(c_str4);

    /*set FEC rate*/
    QString LENH5;
    int _cmd5=Check_FEC_Rate(ui->comboBox_Mod_FECR->currentText(),ui->comboBox_Mod_FECT->currentText(),ui->comboBox_Mod_FECO->currentText());
    switch (_cmd5) {
    case 0: LENH5="m500ctl 0 mdc 0\n";break;
    case 1: LENH5="m500ctl 0 mdc 1\n";break;
    case 2: LENH5="m500ctl 0 mdc 2\n";break;
    case 3: LENH5="m500ctl 0 mdc 3\n";break;
    case 4: LENH5="m500ctl 0 mdc 4\n";break;
    case 5: LENH5="m500ctl 0 mdc 5\n";break;
    case 6: LENH5="m500ctl 0 mdc 6\n";break;
    case 7: LENH5="m500ctl 0 mdc 7\n";break;
    case 8: LENH5="m500ctl 0 mdc 8\n";break;
    }
    QByteArray ba5=LENH5.toLatin1();
    const char *c_str5=ba5.data();
    port->write(c_str5);

    /*Set Scramber*/
    QString LENH6;
    int _cmd6=Check_Scamber(ui->comboBox_Mod_Scram->currentText());
    switch(_cmd6){
        case 0: LENH6="m500ctl 0 mds 0\n";break;
        case 1: LENH6="m500ctl 0 mds 1\n";break;
        case 2: LENH6="m500ctl 0 mds 2\n";break;
        case 3: LENH6="m500ctl 0 mds 3\n";break;
        case 4: LENH6="m500ctl 0 mds 4\n";break;
        case 5: LENH6="m500ctl 0 mds 5\n";break;
        case 6: LENH6="m500ctl 0 mds 6\n";break;
        case 7: LENH6="m500ctl 0 mds 7\n";break;
        case 8: LENH6="m500ctl 0 mds 8\n";break;
        case 9: LENH6="m500ctl 0 mds 9\n";break;
    }
    QByteArray ba6=LENH6.toLatin1();
    const char *c_str6=ba6.data();
    port->write(c_str6);

    /*set Clock source*/

    QString LENH7;
    int _cmd7=Check_Mod_Clock_Source(ui->comboBox_Mod_Clock->currentText());
    switch(_cmd7){
        case 0: LENH7="m500ctl 0 mdk 0\n";break;
        case 1: LENH7="m500ctl 0 mdk 1\n";break;
        case 2: LENH7="m500ctl 0 mdk 2\n";break;
        case 3: LENH7="m500ctl 0 mdk 3\n";break;
    }
    QByteArray ba7=LENH7.toLatin1();
    const char *c_str7=ba7.data();
    port->write(c_str7);

    /*Set Reed Solomon Mode*/
    QString LENH8;
    QString n,d,k;

    int _cmd8=Check_Mode(ui->comboBox_Mod_Mode->currentText());
    switch(_cmd8){
            case 0: {
                        LENH8="m500ctl 0 mdrs 0\n";break;
                        }
            case 1: {
                        LENH8="m500ctl 0 mdrs 1\n";break;
                        }
            case 2: {
                        LENH8="m500ctl 0 mdrs 2\n";break;
                        }
            case 3: {
                        LENH8="m500ctl 0 mdrs 3\n";break;
                        }
            case 4: {
                         n=ui->lineEdit_Mod_n->text();
                         k=ui->lineEdit_Mod_k->text();
                         d=ui->comboBox_Mod_depth->currentText();
                         LENH8="m500ctl 0 mdrs 4\n m500ctl 0 mdrp "+n+","+k+","+d+"\n";
                         break;
                    }
            case 5: {LENH8="m500ctl 0 mdrs 5\n";break;}
    }
    QByteArray ba8=LENH8.toLatin1();
    const char *c_str8=ba8.data();
    port->write(c_str8);

    /*Set Spectrum*/
    QString LENH9;
    int _cmd9=Check_Dir(ui->comboBox_Mod_Dir->currentText());
    switch(_cmd9){
        case 0: LENH9="m500ctl 0 mis 0\n";break;
        case 1: LENH9="m500ctl 0 mis 1\n";break;
    }
    QByteArray ba9=LENH9.toLatin1();
    const char *c_str9=ba9.data();
    port->write(c_str9);

    /*Set Filter m500ctl 0 mifl 1*/
    QString LENH10;
    int _cmd10=Check_Filter(ui->comboBox_Mod_Filter->currentText());
    switch(_cmd10){
        case 0: LENH10="m500ctl 0 mifl 0\n";break;
        case 1: LENH10="m500ctl 0 mifl 1\n";break;
    }
    QByteArray ba10=LENH10.toLatin1();
    const char *c_str10=ba10.data();
    port->write(c_str10);

    /*set Mute m500ctl 0 mit ?*/
    QString LENH11;
    int _cmd11=Check_Mute(ui->comboBox_Mod_Mute->currentText());
    switch(_cmd11){
        case 0: LENH11="m500ctl 0 mit 0\n";break;
        case 1: LENH11="m500ctl 0 mit 1\n";break;
        case 2: LENH11="m500ctl 0 mit 2\n";break;
        case 3: LENH11="m500ctl 0 mit 3\n";break;
    }
    QByteArray ba11=LENH11.toLatin1();
    const char *c_str11=ba11.data();
    port->write(c_str11);

    /*Set Output*/
    QString LENH12;
    int _cmd12=Check_Output(ui->comboBox_Mod_Output->currentText());
    switch(_cmd12){
    // cho nay rat la vo van: nguoc chieu,tim mai moi ra,haizzz
        case 0: LENH12="m500ctl mie 1\n";break;
        case 1: LENH12="m500ctl mie 0\n";break;
    }
    QByteArray ba12=LENH12.toLatin1();
    const char *c_str12=ba12.data();
    port->write(c_str12);

    /*set AUPC m500ctl 0 mip */
    QString LENH13;
    int _cmd13=Check_AUPC(ui->comboBox_Mod_AUPC->currentText());
    switch(_cmd13){
        case 0: LENH13="m500ctl 0 mip 0\n";break;
        case 1: LENH13="m500ctl 0 mip 1\n";break;
        case 2: LENH13="m500ctl 0 mip 2\n";break;
    }
    QByteArray ba13=LENH13.toLatin1();
    const char *c_str13=ba13.data();
    port->write(c_str13);




    /*Set Frequency*/

    /*set CXR offset  m500ctl mio*/

    QString cxrofset=ui->lineEdit_Mod_CxrO->text();

    double _cxrofset=cxrofset.toDouble();
    _cxrofset=_cxrofset*1000;
    QString LENH17="m500ctl mio "+QString::number(_cxrofset)+"\n";
    QByteArray ba17=LENH17.toLatin1();
    const char *c_str17=ba17.data();
    port->write(c_str17);

    /*set IF*/
    bool _ok;
    QString _mif=ui->lineEdit_Mod_IF->text();
    int __mif=_mif.toInt(&_ok,10);
    __mif=__mif*1000000;
    QString LENH15="m500ctl mif "+QString::number(__mif)+"\n";
    QByteArray ba15=LENH15.toLatin1();
    const char *c_str15=ba15.data();
    port->write(c_str15);

    /*set Cxr lvlsss*/
    QString _cxrlvl=ui->lineEdit_Mod_CxrLvL->text();
    int __cxrlvl=_cxrlvl.toInt(&_ok,10);
    __cxrlvl=__cxrlvl*10;
    QString LENH16="m500ctl mil "+QString::number(__cxrlvl)+"\n";
    QByteArray ba16=LENH16.toLatin1();
    const char *c_str16=ba16.data();
    port->write(c_str16);

    QString LENH="m500ctl  mipc ?\n";
    QByteArray ba=LENH.toLatin1();
    const char *c_str=ba.data();
    port->write(c_str);

    /*set gia tri AUPC m500ctl 0 mipc 50,40,-350*/
    QString _ebno=ui->lineEdit_Mod_EbNo->text();
    double x=_ebno.toDouble();x=x*10;
    QString _minlv=ui->lineEdit_Mod_MinL->text();
    double y=_minlv.toDouble();y=y*10;
    QString _maxlv=ui->lineEdit_Mod_MaxL->text();
    double z=_maxlv.toDouble();z=z*10;

    QString LENH14;
    LENH14="m500ctl mipc "+QString::number(x)+","+QString::number(z)+","+QString::number(y)+"\n";
    QByteArray ba14=LENH14.toLatin1();
    const char *c_str14=ba14.data();
    port->write(c_str14);


}


void Dialog::on_pushButton_4_clicked()
{
    //Set DeMod
    /*Set Data Rate*/
     QString LENH1;
     QString _cmd1=ui->lineEdit_DeMod_DR->text();
     LENH1="mq ddr "+_cmd1+"\n";
     QByteArray ba1 = LENH1.toLatin1();
     const char *c_str1 = ba1.data();
     port->write(c_str1);

     /*Set Modulation Mode*/
     QString LENH2;
     int _intcmd=CheckcmbModulation_1(ui->comboBox_DeMod_MM->currentText());
     switch(_intcmd){
     case 0: LENH2="m500ctl 0 ddm 0\n";break;
     case 1: LENH2="m500ctl 0 ddm 1\n";break;
     case 2: LENH2="m500ctl 0 ddm 2\n";break;
     case 3: LENH2="m500ctl 0 ddm 3\n";break;
     case 4: LENH2="m500ctl 0 ddm 4\n";break;
     case 6: LENH2="m500ctl 0 ddm 6\n";break;
     default: break;
     }
     QByteArray ba2=LENH2.toLatin1();
     const char *c_str2=ba2.data();
     port->write(c_str2);

     /*Set FEC Type*/

     QString LENH3;

     int _cmd3=Check_FEC_Type(ui->comboBox_DeMod_FECT->currentText());
     switch(_cmd3){
         case 0: LENH3="m500ctl 0 ddf 0\n";break;
         case 1: LENH3="m500ctl 0 ddf 1\n";break;
         case 2: LENH3="m500ctl 0 ddf 2\n";break;
         case 4: LENH3="m500ctl 0 ddf 4\n";break;
         case 5: LENH3="m500ctl 0 ddf 5\n";break;
         case 6: LENH3="m500ctl 0 ddf 6\n";break;
     }
     QByteArray ba3=LENH3.toLatin1();
     const char *c_str3=ba3.data();
     port->write(c_str3);

     /*set FEC Option*/

     QString LENH4;

     int _cmd4=Check_FEC_Option(ui->comboBox_DeMod_FECO->currentText());
     switch(_cmd4){
         case 0: LENH4="m500ctl 0 ddo 0\n";break;
         case 1: LENH4="m500ctl 0 ddo 1\n";break;
         case 2: LENH4="m500ctl 0 ddo 2\n";break;
         case 3: LENH4="m500ctl 0 ddo 3\n";break;
         case 4: LENH4="m500ctl 0 ddo 4\n";break;
         case 5: LENH4="m500ctl 0 ddo 5\n";break;
         case 6: LENH4="m500ctl 0 ddo 6\n";break;
     }
     QByteArray ba4=LENH4.toLatin1();
     const char *c_str4=ba4.data();
     port->write(c_str4);

     /*set FEC rate*/
     QString LENH5;
     int _cmd5=Check_FEC_Rate(ui->comboBox_DeMod_FECR->currentText(),ui->comboBox_DeMod_FECT->currentText(),ui->comboBox_DeMod_FECO->currentText());
     switch (_cmd5) {
     case 0: LENH5="m500ctl 0 ddc 0\n";break;
     case 1: LENH5="m500ctl 0 ddc 1\n";break;
     case 2: LENH5="m500ctl 0 ddc 2\n";break;
     case 3: LENH5="m500ctl 0 ddc 3\n";break;
     case 4: LENH5="m500ctl 0 ddc 4\n";break;
     case 5: LENH5="m500ctl 0 ddc 5\n";break;
     case 6: LENH5="m500ctl 0 ddc 6\n";break;
     case 7: LENH5="m500ctl 0 ddc 7\n";break;
     case 8: LENH5="m500ctl 0 ddc 8\n";break;
     }
     QByteArray ba5=LENH5.toLatin1();
     const char *c_str5=ba5.data();
     port->write(c_str5);

     /*Set Scramber*/
     QString LENH6;
     int _cmd6=Check_Scamber(ui->comboBox_DeMod_Scram->currentText());
     switch(_cmd6){
         case 0: LENH6="m500ctl 0 dds 0\n";break;
         case 1: LENH6="m500ctl 0 dds 1\n";break;
         case 2: LENH6="m500ctl 0 dds 2\n";break;
         case 3: LENH6="m500ctl 0 dds 3\n";break;
         case 4: LENH6="m500ctl 0 dds 4\n";break;
         case 5: LENH6="m500ctl 0 dds 5\n";break;
         case 6: LENH6="m500ctl 0 dds 6\n";break;
         case 7: LENH6="m500ctl 0 dds 7\n";break;
         case 8: LENH6="m500ctl 0 dds 8\n";break;
         case 9: LENH6="m500ctl 0 dds 9\n";break;
     }
     QByteArray ba6=LENH6.toLatin1();
     const char *c_str6=ba6.data();
     port->write(c_str6);

     /*set Clock source*/

     QString LENH7;
     int _cmd7=Check_Demod_Clock_Source(ui->comboBox_DeMod_Clock->currentText());
     switch(_cmd7){
         case 0: LENH7="m500ctl 0 ddk 0\n";break;
         case 1: LENH7="m500ctl 0 ddk 1\n";break;
         case 2: LENH7="m500ctl 0 ddk 2\n";break;
         case 3: LENH7="m500ctl 0 ddk 3\n";break;
     }
     QByteArray ba7=LENH7.toLatin1();
     const char *c_str7=ba7.data();
     port->write(c_str7);

     /*Set Reed Solomon Mode*/
     QString LENH8;
     QString n,d,k;

     int _cmd8=Check_Mode(ui->comboBox_DeMod_Mode->currentText());
     switch(_cmd8){
             case 0: {
                         LENH8="m500ctl 0 ddrs 0\n";break;
                         }
             case 1: {
                         LENH8="m500ctl 0 ddrs 1\n";break;
                         }
             case 2: {
                         LENH8="m500ctl 0 ddrs 2\n";break;
                         }
             case 3: {
                         LENH8="m500ctl 0 ddrs 3\n";break;
                         }
             case 4: {
                          n=ui->lineEdit_DeMod_n->text();
                          k=ui->lineEdit_DeMod_k->text();
                          d=ui->comboBox_DeMod_depth->currentText();
                          LENH8="m500ctl 0 ddrs 4\n m500ctl 0 ddrp "+n+","+k+","+d+"\n";
                          break;
                     }
             case 5: {LENH8="m500ctl 0 ddrs 5\n";break;}
     }
     QByteArray ba8=LENH8.toLatin1();
     const char *c_str8=ba8.data();
     port->write(c_str8);

     /*set IF*/
     /*Lenh ktra
     QString LENH="m500ctl dif ?\n";
     QByteArray ba=LENH.toLatin1();
     const char *c_str=ba.data();
     port->write(c_str);*/

     bool _ok;
     QString _mif=ui->lineEdit_DeMod_IF->text();
     int __mif=_mif.toInt(&_ok,10);
     __mif=__mif*1000000;
     QString LENH15="m500ctl dif "+QString::number(__mif)+"\n";
     QByteArray ba15=LENH15.toLatin1();
     const char *c_str15=ba15.data();
     port->write(c_str15);

     /*Set Spectrum*/
     QString LENH9;
     int _cmd9=Check_Dir(ui->comboBox_DeMod_Dir->currentText());
     switch(_cmd9){
         case 0: LENH9="m500ctl 0 dis 0\n";break;
         case 1: LENH9="m500ctl 0 dis 1\n";break;
     }
     QByteArray ba9=LENH9.toLatin1();
     const char *c_str9=ba9.data();
     port->write(c_str9);

     /*Set Filter m500ctl 0 difl 1*/
     QString LENH10;
     int _cmd10=Check_Filter(ui->comboBox_DeMod_Filter->currentText());
     switch(_cmd10){
         case 0: LENH10="m500ctl 0 difl 0\n";break;
         case 1: LENH10="m500ctl 0 difl 1\n";break;
     }
     QByteArray ba10=LENH10.toLatin1();
     const char *c_str10=ba10.data();
     port->write(c_str10);

     /*Set Sweep Range*/
     QString LENH11;
     QString _sweep=ui->lineEdit_DeMod_Sweep->text();
     double __sweep=_sweep.toDouble();
     __sweep=__sweep*1000;
     LENH11="m500ctl dia "+QString::number(__sweep)+"\n";
     QByteArray ba11=LENH11.toLatin1();
     const char *c_str11=ba11.data();
     port->write(c_str11);

     /*Set Cxr Lvl*/
     QString _cxr=ui->lineEdit_DeMod_CxrL->text();
     double __cxr=_cxr.toDouble();
     QString LENH12="m500ctl dil "+QString::number(__cxr)+"\n";
     QByteArray ba12=LENH12.toLatin1();
     const char *c_str12=ba12.data();
     port->write(c_str12);

     /*Set Eb/No*/

     QString _ebno=ui->lineEdit_DeMod_EbNo->text();
     double __ebno=_ebno.toDouble();
     QString LENH13="m500ctl die "+QString::number(__ebno)+"\n";
     QByteArray ba13=LENH13.toLatin1();
     const char *c_str13=ba13.data();
     port->write(c_str13);
     //den day roi nhe
}

void Dialog::on_comboBox_Mod_FECO_currentIndexChanged(const QString &arg1)
{
    QString a=ui->comboBox_Mod_FECT->currentText();
    QString b=ui->comboBox_Mod_FECO->currentText();
    if(a=="UnCoded"){
        while(ui->comboBox_Mod_FECR->count()>0){
            ui->comboBox_Mod_FECR->removeItem(0);
        }
        if(b=="None")
        ui->comboBox_Mod_FECR->addItem("N/A");
   }
    else if(a=="Viterbi"){
        while(ui->comboBox_Mod_FECR->count()>0){
            ui->comboBox_Mod_FECR->removeItem(0);
        }
        if(b=="Normal"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("5/6");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
        else if(b=="Swap C0/C1"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("5/6");
            ui->comboBox_Mod_FECR->addItem("7/8");}
        else if(b=="CT"){
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("7/8");
            }

   }
    else if(a=="TCM"){
        while(ui->comboBox_Mod_FECR->count()>0){
            ui->comboBox_Mod_FECR->removeItem(0);
        }
        if(b=="Normal"){
            ui->comboBox_Mod_FECR->addItem("2/3");

        }
   }
    else if(a=="TPC"){
        while(ui->comboBox_Mod_FECR->count()>0){
            ui->comboBox_Mod_FECR->removeItem(0);
        }
        if(b=="Advanced"){
            ui->comboBox_Mod_FECR->addItem("0.453-16K");
            ui->comboBox_Mod_FECR->addItem("1/2-16K");
            ui->comboBox_Mod_FECR->addItem("1/2-4K");
            ui->comboBox_Mod_FECR->addItem("3/4-16K");
            ui->comboBox_Mod_FECR->addItem("3/4-4K");
            ui->comboBox_Mod_FECR->addItem("7/8-16K");
            ui->comboBox_Mod_FECR->addItem("7/8-4K");
            ui->comboBox_Mod_FECR->addItem("0.922-16K");
            ui->comboBox_Mod_FECR->addItem("0.950-4K");
        }
        else if(b=="M5 Full"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
        else if(b=="M5 Short"){
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
        else if(b=="M5 Legacy"){
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
        else if(b=="CT"){
            ui->comboBox_Mod_FECR->addItem("5/16");
            ui->comboBox_Mod_FECR->addItem("21/44");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("0.95");
        }
   }
    else if(a=="LDPC"){
        while(ui->comboBox_Mod_FECR->count()>0){
            ui->comboBox_Mod_FECR->removeItem(0);
        }
        if(b=="256 Block"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("2/3");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("14/17");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("10/11");
            ui->comboBox_Mod_FECR->addItem("16/17");
        }
        else if(b=="512 Block"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("2/3");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("14/17");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("10/11");
            ui->comboBox_Mod_FECR->addItem("16/17");
        }
        else if(b=="1K Block"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("2/3");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("14/17");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("10/11");
            ui->comboBox_Mod_FECR->addItem("16/17");
        }
        else if(b=="2K Block"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("2/3");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("14/17");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("10/11");
            ui->comboBox_Mod_FECR->addItem("16/17");
        }
        else if(b=="4K Block"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("2/3");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("14/17");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("10/11");
            ui->comboBox_Mod_FECR->addItem("16/17");
        }
        else if(b=="8K Block"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("2/3");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("14/17");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("10/11");
            ui->comboBox_Mod_FECR->addItem("16/17");
        }
        else if(b=="16K Block"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("2/3");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("14/17");
            ui->comboBox_Mod_FECR->addItem("7/8");
            ui->comboBox_Mod_FECR->addItem("10/11");
            ui->comboBox_Mod_FECR->addItem("16/17");
        }
    }
    else if(a=="S-Tec"){
        while(ui->comboBox_Mod_FECR->count()>0){
            ui->comboBox_Mod_FECR->removeItem(0);}
        if(b=="Large"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("3/5");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("4/5");
            ui->comboBox_Mod_FECR->addItem("5/6");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
        else if(b=="Medium"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("3/5");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("4/5");
            ui->comboBox_Mod_FECR->addItem("5/6");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
        else if(b=="Small"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("3/5");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("4/5");
            ui->comboBox_Mod_FECR->addItem("5/6");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
        else if(b=="XLarge"){
            ui->comboBox_Mod_FECR->addItem("1/2");
            ui->comboBox_Mod_FECR->addItem("3/5");
            ui->comboBox_Mod_FECR->addItem("3/4");
            ui->comboBox_Mod_FECR->addItem("4/5");
            ui->comboBox_Mod_FECR->addItem("5/6");
            ui->comboBox_Mod_FECR->addItem("7/8");
        }
    }
}

void Dialog::on_comboBox_DeMod_FECT_currentIndexChanged(const QString &arg1)
{
    QString a=ui->comboBox_DeMod_FECT->currentText();

    if(a=="UnCoded"){
        while(ui->comboBox_DeMod_FECO->count()>0){
            ui->comboBox_DeMod_FECO->removeItem(0);
        }
    ui->comboBox_DeMod_FECO->addItem("None");
   }
    else if(a=="Viterbi"){
        while(ui->comboBox_DeMod_FECO->count()>0){
            ui->comboBox_DeMod_FECO->removeItem(0);
        }
        ui->comboBox_DeMod_FECO->addItem("Normal");
        ui->comboBox_DeMod_FECO->addItem("Swap C0/C1");
        ui->comboBox_DeMod_FECO->addItem("CT");
    }
    else if(a=="TCM"){
        while(ui->comboBox_DeMod_FECO->count()>0){
            ui->comboBox_DeMod_FECO->removeItem(0);
        }

        ui->comboBox_DeMod_FECO->addItem("Normal");
    }
    else if(a=="TPC"){
        while(ui->comboBox_DeMod_FECO->count()>0){
            ui->comboBox_DeMod_FECO->removeItem(0);
        }

        ui->comboBox_DeMod_FECO->addItem("Advanced");
        ui->comboBox_DeMod_FECO->addItem("M5 Full");
        ui->comboBox_DeMod_FECO->addItem("M5 Short");
        ui->comboBox_DeMod_FECO->addItem("M5 Legacy");
        ui->comboBox_DeMod_FECO->addItem("CT");
    }
    else if(a=="LDPC"){
        while(ui->comboBox_DeMod_FECO->count()>0){
            ui->comboBox_DeMod_FECO->removeItem(0);
        }

        ui->comboBox_DeMod_FECO->addItem("256 Block");
        ui->comboBox_DeMod_FECO->addItem("512 Block");
        ui->comboBox_DeMod_FECO->addItem("1K Block");
        ui->comboBox_DeMod_FECO->addItem("2K Block");
        ui->comboBox_DeMod_FECO->addItem("4K Block");
        ui->comboBox_DeMod_FECO->addItem("8K Block");
        ui->comboBox_DeMod_FECO->addItem("16K Block");

    }
    else if(a=="S-Tec"){
        while(ui->comboBox_DeMod_FECO->count()>0){
            ui->comboBox_DeMod_FECO->removeItem(0);
        }

        ui->comboBox_DeMod_FECO->addItem("Large");
        ui->comboBox_DeMod_FECO->addItem("Medium");
        ui->comboBox_DeMod_FECO->addItem("Small");
        ui->comboBox_DeMod_FECO->addItem("XLarge");
    }
    qDebug()<<a;
}

void Dialog::on_comboBox_DeMod_FECO_currentIndexChanged(const QString &arg1)
{
    QString a=ui->comboBox_DeMod_FECT->currentText();
    QString b=ui->comboBox_DeMod_FECO->currentText();
    if(a=="UnCoded"){
        while(ui->comboBox_DeMod_FECR->count()>0){
            ui->comboBox_DeMod_FECR->removeItem(0);
        }
        if(b=="None")
        ui->comboBox_DeMod_FECR->addItem("N/A");
   }
    else if(a=="Viterbi"){
        while(ui->comboBox_DeMod_FECR->count()>0){
            ui->comboBox_DeMod_FECR->removeItem(0);
        }
        if(b=="Normal"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("5/6");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
        else if(b=="Swap C0/C1"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("5/6");
            ui->comboBox_DeMod_FECR->addItem("7/8");}
        else if(b=="CT"){
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            }

   }
    else if(a=="TCM"){
        while(ui->comboBox_DeMod_FECR->count()>0){
            ui->comboBox_DeMod_FECR->removeItem(0);
        }
        if(b=="Normal"){
            ui->comboBox_DeMod_FECR->addItem("2/3");

        }
   }
    else if(a=="TPC"){
        while(ui->comboBox_DeMod_FECR->count()>0){
            ui->comboBox_DeMod_FECR->removeItem(0);
        }
        if(b=="Advanced"){
            ui->comboBox_DeMod_FECR->addItem("0.453-16K");
            ui->comboBox_DeMod_FECR->addItem("1/2-16K");
            ui->comboBox_DeMod_FECR->addItem("1/2-4K");
            ui->comboBox_DeMod_FECR->addItem("3/4-16K");
            ui->comboBox_DeMod_FECR->addItem("3/4-4K");
            ui->comboBox_DeMod_FECR->addItem("7/8-16K");
            ui->comboBox_DeMod_FECR->addItem("7/8-4K");
            ui->comboBox_DeMod_FECR->addItem("0.922-16K");
            ui->comboBox_DeMod_FECR->addItem("0.950-4K");
        }
        else if(b=="M5 Full"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
        else if(b=="M5 Short"){
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
        else if(b=="M5 Legacy"){
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
        else if(b=="CT"){
            ui->comboBox_DeMod_FECR->addItem("5/16");
            ui->comboBox_DeMod_FECR->addItem("21/44");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("0.95");
        }
   }
    else if(a=="LDPC"){
        while(ui->comboBox_DeMod_FECR->count()>0){
            ui->comboBox_DeMod_FECR->removeItem(0);
        }
        if(b=="256 Block"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("2/3");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("14/17");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("10/11");
            ui->comboBox_DeMod_FECR->addItem("16/17");
        }
        else if(b=="512 Block"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("2/3");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("14/17");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("10/11");
            ui->comboBox_DeMod_FECR->addItem("16/17");
        }
        else if(b=="1K Block"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("2/3");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("14/17");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("10/11");
            ui->comboBox_DeMod_FECR->addItem("16/17");
        }
        else if(b=="2K Block"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("2/3");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("14/17");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("10/11");
            ui->comboBox_DeMod_FECR->addItem("16/17");
        }
        else if(b=="4K Block"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("2/3");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("14/17");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("10/11");
            ui->comboBox_DeMod_FECR->addItem("16/17");
        }
        else if(b=="8K Block"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("2/3");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("14/17");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("10/11");
            ui->comboBox_DeMod_FECR->addItem("16/17");
        }
        else if(b=="16K Block"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("2/3");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("14/17");
            ui->comboBox_DeMod_FECR->addItem("7/8");
            ui->comboBox_DeMod_FECR->addItem("10/11");
            ui->comboBox_DeMod_FECR->addItem("16/17");
        }
    }
    else if(a=="S-Tec"){
        while(ui->comboBox_DeMod_FECR->count()>0){
            ui->comboBox_DeMod_FECR->removeItem(0);}
        if(b=="Large"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("3/5");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("4/5");
            ui->comboBox_DeMod_FECR->addItem("5/6");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
        else if(b=="Medium"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("3/5");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("4/5");
            ui->comboBox_DeMod_FECR->addItem("5/6");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
        else if(b=="Small"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("3/5");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("4/5");
            ui->comboBox_DeMod_FECR->addItem("5/6");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
        else if(b=="XLarge"){
            ui->comboBox_DeMod_FECR->addItem("1/2");
            ui->comboBox_DeMod_FECR->addItem("3/5");
            ui->comboBox_DeMod_FECR->addItem("3/4");
            ui->comboBox_DeMod_FECR->addItem("4/5");
            ui->comboBox_DeMod_FECR->addItem("5/6");
            ui->comboBox_DeMod_FECR->addItem("7/8");
        }
    }
}

void Dialog::on_comboBox_Mod_Mode_currentIndexChanged(const QString &arg1)
{
    QString a=ui->comboBox_Mod_Mode->currentText();
    if(a=="Disabled"|| a=="IESS-308" || a=="IESS-309" || a=="IESS-310" ||a=="CT 220/200")
    {
        ui->lineEdit_Mod_n->clear();
        ui->lineEdit_Mod_k->clear();
        ui->lineEdit_Mod_n->setReadOnly(true);
        ui->lineEdit_Mod_k->setReadOnly(true);
    }
    else {
        ui->lineEdit_Mod_n->setReadOnly(false);
        ui->lineEdit_Mod_k->setReadOnly(false);
    }
}

void Dialog::on_comboBox_DeMod_Mode_currentIndexChanged(const QString &arg1)
{
    QString a=ui->comboBox_DeMod_Mode->currentText();
    if(a=="Disabled"|| a=="IESS-308" || a=="IESS-309" || a=="IESS-310" ||a=="CT 220/200")
    {
        ui->lineEdit_DeMod_n->clear();
        ui->lineEdit_DeMod_k->clear();
        ui->lineEdit_DeMod_n->setReadOnly(true);
        ui->lineEdit_DeMod_k->setReadOnly(true);
    }
    else {
        ui->lineEdit_DeMod_n->setReadOnly(false);
        ui->lineEdit_DeMod_k->setReadOnly(false);
    }
}
