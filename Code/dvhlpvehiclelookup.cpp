#include "DVHlpvehiclelookup.h"
#include "ui_DVUI_LeftPanelVehicleLookup.h"

//CHANGES_Below includes have been updated with <> instead of " " to avoid unnecessary lookup of files into current directory.
#include <QMessageBox>
#include <QFile>
#include <QFileInfo>
#include <QStringListModel>



DVHlpvehiclelookup::DVHlpvehiclelookup(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DVUI_LeftPanelVehicleLookup)
{
    // Check the database ,CHANGES_H in CHeck corrected to check.
    const QString DRIVER("QSQLITE");
    if(QSqlDatabase::isDriverAvailable(DRIVER))
    {
        qDebug() << "SQLITE Driver : Found!" << endl;
        vehicle_database_  = QSqlDatabase::addDatabase(DRIVER);
        vehicle_database_.setDatabaseName(":memory:");
    }
    else
    {
        qDebug() << "SQLITE Driver : not Found!" << endl;
    }

    ui->setupUi(this);
}

DVHlpvehiclelookup::~DVHlpvehiclelookup()
{
    //CHANGES_Code has been updated while  maintaining consistency especially in the  spaces.
    if ( vehicle_database_.isOpen() )
        qDebug() << "The database is:  Open" << endl;
    else
        qDebug() << "The database is:  Closed" << endl;

    vehicle_database_.removeDatabase( QSqlDatabase::defaultConnection );
    delete ui;

}

void DVHlpvehiclelookup::SetSourceFile(const QString &file)
{
    file_ = file;
}

void DVHlpvehiclelookup::UpdateFromFile()
{

    if(!UpdateDatabase())
    {
        //CHANGES_A pop up message added to inform the user about the failure of UpdateDatabase().
        qDebug() << "ERROR: Reading CSV to SQL Database Failed! " << endl;
        QMessageBox msg;
        msg.setText("ERROR: Failed to read the file! Try restarting the application.");
        msg.exec();
        return;

    }

    UpdateUI();


}


void DVHlpvehiclelookup::on_comboBox_year_currentTextChanged(const QString &year)
{
    UpdateUIMake(year);
}


void DVHlpvehiclelookup::on_comboBox_make_currentTextChanged(const QString &make)
{
    UpdateUIModel(make);

}


void DVHlpvehiclelookup::on_pushButton_submit_clicked()
{

    SetUIText("OL",ui->lineEdit_OL);
    SetUIText("OW",ui->lineEdit_OW);
    SetUIText("OH",ui->lineEdit_OH);
    SetUIText("WB",ui->lineEdit_WB);
    SetUIText("CW",ui->lineEdit_CW);
    SetUIText("A1",ui->lineEdit_A1);
    SetUIText("B1",ui->lineEdit_B1);
    SetUIText("C1",ui->lineEdit_C1);
    SetUIText("D1",ui->lineEdit_D1);
    SetUIText("E1",ui->lineEdit_E1);
    SetUIText("F1",ui->lineEdit_F1);
    SetUIText("G1",ui->lineEdit_G1);

}


bool DVHlpvehiclelookup::UpdateDatabase()
{
    if(!vehicle_database_.open ())
    {
        qDebug() << "ERROR: DataBase not open!" << endl;
        return  false;
    }

    if(!QFileInfo(file_).exists())
    {
        qDebug() << "ERROR: File path does not exist!" << endl;
        return  false;
    }

    QSqlQuery query;
    if(!query.exec("DROP TABLE VehicleInfo;"))
        qDebug() << "WARNING: Drop Vehicle Info Table Failed!" << endl;


    //Read the File
    QFile f_handle(file_);
    if(f_handle.open (QIODevice::ReadOnly))
    {
        query.clear();
        QTextStream ts (&f_handle);

        //Travel through the csv file
        //Read the firt line and create a table with the details from the file as column titles.
        // 2nd Row onwards simply keep updating the tables with the data from the file .
        unsigned int iteration_count = 0;
        QString first_line_backup;
        while(!ts.atEnd()){

            // Get the line and extract the data from it
            QString line = ts.readLine();
            qDebug()<<"CHECK LINE:"<<line<<endl;

            QStringList list_values = line.split(",");
            //If it is the first line save it as a column header names
            if(iteration_count == 0)
            {

                //First line backup
                first_line_backup = line;

                //Prepare the create table query string to support dynamic table creation (based on n columns in the file) if it is the 1st row data
                QString query_string = "CREATE TABLE VehicleInfo ("+ line +")";

                //Prepare table query
                query.prepare(query_string);

            }
            else
            {
                /*Insert values query
                Prepare the Insert data to table query string to support dynamic table data insertion
                (based on n columns in the file) if it is the 1st row data*/
                QString query_string = "INSERT INTO VehicleInfo ("+ first_line_backup +")VALUES (";

                QStringList list_column_title = first_line_backup.split(",");

                for(auto const &value :list_column_title)
                {
                    query_string.append(":"+value);
                    //Do not add comma(,)  it is the last element . This avoids need for chopping .
                    if(value != list_column_title.at(list_column_title.size()-1))
                        query_string.append(", ");
                }
                query_string.append(")");

                //Prepare table query
                query.prepare(query_string);

                //Bind the values now using QSqlquery bind

                if(list_column_title.size()<list_values.size())
                {
                    QMessageBox msg;
                    msg.setText("ERROR: Values are more than number of avaiable columns!Skipping..");
                    msg.exec();
                }
                int iter = 0;
                for(auto const &value :list_values)
                {
                    query.bindValue(":"+list_column_title.at(iter) , value);
                    iter++;
                }

            }

            if(query.exec())
            {
                qDebug() << "Query Success:" <<  query.lastQuery() << endl;
            }
            else
            {
                qDebug() << "Query Failed:" <<    query.lastQuery() << endl;
                QMessageBox msg;
                msg.setText("ERROR: Failed to read  information from the File to database. Please try again");
                msg.exec();

                //incase of any failure do not proceed.
                f_handle.close ();
                return false;//File failed to Open.CHANGES_Better checks added incase of file failing to open.

            }
            query.clear();

            iteration_count++;
        }
        f_handle.close ();
    }
    else
    {
        return false;//File failed to Open.CHANGES_Better checks added incase of file failing to open.
    }

    return true;//CHANGES_Better checks added if every thing goes well return true.
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
    query.prepare("SELECT DISTINCT MYR FROM VehicleInfo");

    if(query.exec())
    {
        qDebug() << "Query Success:" <<  query.lastQuery() << endl;
    }
    else
    {
        qDebug() << "Query Failed:" <<    query.lastQuery() << endl;
        QMessageBox msg;
        msg.setText("ERROR: Failed to query Year information from the database. Please try again");
        msg.exec();
        return ;// Failed Query
    }

    while (query.next())
    {
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

void DVHlpvehiclelookup::UpdateUIMake(const QString &selected_year)
{
    QStringListModel *vmake_data_model = new QStringListModel;
    QStringList list_vmake;
    QSqlQuery query;
    query.prepare("SELECT DISTINCT MAKE "
                  "FROM VehicleInfo WHERE MYR='"+selected_year+"'");

    if(query.exec())
    {
        qDebug() << "Query Success:" <<  query.lastQuery() << endl;
    }
    else
    {
        qDebug() << "Query Failed:" <<    query.lastQuery() << endl;
        QMessageBox msg;
        msg.setText("ERROR: Failed to query make information from the database. Please try again");
        msg.exec();
        return ;// Failed Query
    }
    while (query.next())
    {
        list_vmake.append(query.value(0).toString());
    }

    vmake_data_model->setStringList(list_vmake);
    ui->comboBox_make->setModel(vmake_data_model);
}

void DVHlpvehiclelookup::UpdateUIModel(const QString &selected_make)
{
    QStringListModel *model_data_model = new QStringListModel;
    QStringList list_model;
    QSqlQuery query;
    query.prepare("SELECT DISTINCT MODEL FROM VehicleInfo "
                  "WHERE MYR='" + ui->comboBox_year->currentText() + "'"
                                                                     "AND MAKE='" + selected_make + "'" );
    if(query.exec())
    {
        qDebug() << "Query Success:" <<  query.lastQuery() << endl;
    }
    else
    {
        qDebug() << "Query Failed:" <<    query.lastQuery() << endl;
        QMessageBox msg;
        msg.setText("ERROR: Failed to query Model information from the database. Please try again");
        msg.exec();
        return ;// Failed Query
    }

    while (query.next())
    {
        list_model.append(query.value(0).toString());
    }


    model_data_model->setStringList(list_model);
    ui->comboBox_model->setModel(model_data_model);
}

//CHANGES_A dedicated function added with SQL query not with QString but directly using Qsql prepare method.
void DVHlpvehiclelookup::SetUIText(QString value, QLineEdit *line_edit)
{
    QString year = ui->comboBox_year->currentText();
    QString model = ui->comboBox_model->currentText();
    QString make = ui->comboBox_make->currentText() ;
    QString result;
    QSqlQuery query;
    query.prepare("SELECT " + value +
                  " FROM VehicleInfo"
                  " WHERE MYR='"+ year + "'"
                                         " AND MAKE='"+ make  + "'"
                                                                " AND MODEL='" + model + "'");
    query.exec();
    while (query.next())
    {
        result = query.value(0).toString();
    }

    // After the query update the UI
    line_edit->setText(result);


}




