#include "DVHlpvehiclelookup.h"
#include "ui_DVUI_LeftPanelVehicleLookup.h"
#include "QMessageBox"
#include "QFile"
#include "QFileInfo"
#include "QStringListModel"



DVHlpvehiclelookup::DVHlpvehiclelookup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DVUI_LeftPanelVehicleLookup)
{
    // CHeck the database
    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER))
    {
        qDebug()<<"SQLITE Driver : Found!"<<endl;
        vehicle_database_  = QSqlDatabase::addDatabase(DRIVER);
        vehicle_database_.setDatabaseName(":memory:");

    }
    else {
        qDebug()<<"SQLITE Driver : not Found!"<<endl;
    }

    ui->setupUi(this);
}

DVHlpvehiclelookup::~DVHlpvehiclelookup()
{
    if ( vehicle_database_.isOpen() )
        qDebug() << "the database is:  open" <<endl;
    else
        qDebug()<< "the database is:  Closed" <<endl;

    vehicle_database_.removeDatabase( QSqlDatabase::defaultConnection );
    delete ui;
}

void DVHlpvehiclelookup::SetSourceFile(const QString &file)
{
    file_ = file;
}

void DVHlpvehiclelookup::Update()
{

    if(!UpdateDatabase())
    {
        qDebug()<<"ERROR: Reading CSV to SQL Database Failed! "<<endl;
        return;
    }

    UpdateUI();


}




void DVHlpvehiclelookup::on_comboBox_year_currentTextChanged(const QString &year)
{
    UpdateUIModel(year);
}

void DVHlpvehiclelookup::on_comboBox_model_currentTextChanged(const QString &model)
{
    UpdateUIMake(model);
}

void DVHlpvehiclelookup::on_pushButton_submit_clicked()
{
    // Get the Year
    QString year = ui->comboBox_year->currentText();

    // Get the Model
    QString model = ui->comboBox_model->currentText();

    // Get the Make
    QString make = ui->comboBox_make->currentText();

    //Values
    QString OL_value;
    QString OW_value;
    QString OH_value;
    QString WB_value;
    QString CW_value;
    QString A1_value;
    QString B1_value;
    QString C1_value;
    QString D1_value;
    QString E1_value;
    QString F1_value;
    QString G1_value;


    //Query and Update
    QSqlQuery query;

    QString common_string1 = "SELECT DISTINCT ";
    QString common_string2 = " FROM VehicleInfo "
                             "WHERE MYR='"+year+"' "+
            "AND MODEL='"+model+"' "+
            "AND MAKE='"+make+"'";

    QString query_string = common_string1+"OL"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        OL_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"OW"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        OW_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"OH"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        OH_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"WB"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        WB_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"CW"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        CW_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"A1"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        A1_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"B1"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        B1_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"C1"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        C1_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"D1"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        D1_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"E1"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        E1_value = query.value(0).toString();
    }


    query.clear();
    query_string = common_string1+"F1"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        F1_value = query.value(0).toString();
    }

    query.clear();
    query_string = common_string1+"G1"+common_string2;
    query.exec(query_string);
    while (query.next()) {
        G1_value = query.value(0).toString();
    }

    //Update Values to UI
    ui->lineEdit_OL->setText(OL_value);
    ui->lineEdit_OW->setText(OW_value);
    ui->lineEdit_OH->setText(OH_value);
    ui->lineEdit_WB->setText(WB_value);
    ui->lineEdit_CW->setText(CW_value);
    ui->lineEdit_A1->setText(A1_value);
    ui->lineEdit_B1->setText(B1_value);
    ui->lineEdit_C1->setText(C1_value);
    ui->lineEdit_D1->setText(D1_value);
    ui->lineEdit_E1->setText(E1_value);
    ui->lineEdit_F1->setText(F1_value);
    ui->lineEdit_G1->setText(G1_value);


}


bool DVHlpvehiclelookup::UpdateDatabase()
{
    if(!vehicle_database_.open ()){
        qDebug()<<"ERROR: DataBase not open!"<<endl;
        return  false;
    }

    if(!QFileInfo(file_).exists()){
        qDebug()<<"ERROR: File path does not exist!"<<endl;
        return  false;
    }

    QSqlQuery query;
    if(!query.exec("DROP TABLE VehicleInfo;"))
        qDebug()<<"WARNING: Drop Vehicle Info Table Failed!"<<endl;


    //Read the File
    QFile f_handle(file_);
    if(f_handle.open (QIODevice::ReadOnly)){
        query.clear();
        QTextStream ts (&f_handle);
        QString query_string;

        //Travel through the csv file
        unsigned int iteration_count = 0;
        while(!ts.atEnd()){
            //Get the line
            QStringList line = ts.readLine().split(',');
            if(iteration_count == 0)
            {
                //Create table query
                query_string.append("CREATE TABLE VehicleInfo (");
            }
            else
            {
                //Insert values query
                query_string.append("INSERT INTO VehicleInfo VALUES (");
            }

            /*for every values on a line,
                append it to the INSERT request*/
            for(int i=0; i<line .length ();++i){
                QString value = "'"+line.at(i)+"'";
                query_string.append(value);
                query_string.append(",");
            }
            query_string.chop(1); // remove the trailing comma
            query_string.append(");"); // close the "VALUES([...]" with a ");"
            if(query.exec(query_string))
                qDebug()<<"Query Success:"<<query_string<<endl;
            else
                qDebug()<<"Query Failed:"<<query_string<<endl;


            query_string.clear();

            iteration_count++;
        }
        f_handle.close ();
    }
}

void DVHlpvehiclelookup::UpdateUI()
{
    UpdateUIYear();
}

void DVHlpvehiclelookup::UpdateUIYear()
{
    QStringListModel *year_data_model = new QStringListModel;
    QSqlQuery query;
    QList<unsigned int> list_year_as_num;

    query.exec("SELECT DISTINCT MYR FROM VehicleInfo");
    while (query.next()) {
        list_year_as_num.append(QString(query.value(0).toString()).toUInt());
    }

    //Sorting is done as numbers to cope up with the issue of 3 4 being used instead of 03 and 04(an example).String sorting gives incorrect result.
    qSort(list_year_as_num.begin(),list_year_as_num.end());

    //Back to string for display.
    QStringList list_year_as_string;
    for(auto const & value:list_year_as_num)
    {
        list_year_as_string.push_back(QString::number(value));
    }
    year_data_model->setStringList(list_year_as_string);
    ui->comboBox_year->setModel(year_data_model);
}

void DVHlpvehiclelookup::UpdateUIModel(const QString &selected_year)
{
    QStringListModel *vmodel_data_model = new QStringListModel;
    QStringList list_vmodel;
    QString query_string = "SELECT DISTINCT MODEL FROM VehicleInfo WHERE MYR='"+selected_year+"'";
    QSqlQuery query;
    query.exec(query_string);
    while (query.next()) {
        list_vmodel.append(query.value(0).toString());
    }
    vmodel_data_model->setStringList(list_vmodel);
    ui->comboBox_model->setModel(vmodel_data_model);
}

void DVHlpvehiclelookup::UpdateUIMake(const QString &selected_model)
{
    QStringListModel *make_data_model = new QStringListModel;
    QStringList list_make;
    QString query_string = "SELECT DISTINCT MAKE FROM VehicleInfo WHERE MODEL='"+selected_model+"'";
    QSqlQuery query;
    query.exec(query_string);
    while (query.next()) {
        list_make.append(query.value(0).toString());
    }
    make_data_model->setStringList(list_make);
    ui->comboBox_make->setModel(make_data_model);
}




