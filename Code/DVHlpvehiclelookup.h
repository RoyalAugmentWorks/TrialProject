#ifndef DVHLPVEHICLELOOKUP_H
#define DVHLPVEHICLELOOKUP_H
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>

/*!
 * Qt Version :- 5.13.1
 *
 * File :- DVHlpvehiclelookup.h
 * Last Modified:- 11/04/2019.
 *
 *
 * This file makes use of QSQLite Database to store the information in the memory for quick access.
 * Approach  - Step 1:- Initialize the temporary vehicle_database_ using QSQLITE.
 *           - Step 2:- Read the CSV File and insert the infos to the vehicle_database_.
 *           - Step 3:- Populate the Year Combo Box.
 *                      3.a:- Query the Database to get the year column without duplicates.
 *                      3.b:- Update the values to the comboBox_year.
 *
 *          While Running (When User selects a year) :-
 *              - Step 1:- Get the year value.
 *              - Step 2:- Query the models available in that year value in the vehicle_database_.
 *              - Step 3:- Populate the values in the comboBox_model.
 *
 *         Similar workflow can be observed in all further options.
 */
namespace Ui {
class DVUI_LeftPanelVehicleLookup;
}
/*!
 * \brief The DVHlpvehiclelookup class -A widget that basically reads a csv file, backsup in a temp database and displays in the GUI interface
 */
class DVHlpvehiclelookup : public QWidget
{
    Q_OBJECT
public:
    explicit DVHlpvehiclelookup(QWidget *parent = nullptr);
    ~DVHlpvehiclelookup();

    /*!
     * \brief SetSourceFile - a method to set the filepath from which the data has to be populated.
     * \param file - csv file path
     */
    void SetSourceFile(const QString &file);

    /*!
     * \brief Update - Read the File, Populate the database, Update the UI
     */
    void Update();

private slots:
    void on_comboBox_year_currentTextChanged(const QString &arg1);
    void on_comboBox_model_currentTextChanged(const QString &arg1);
    void on_pushButton_submit_clicked();

private:
    /*!
     * \brief UpdateDatabase - Read the CSV file and updates the temp database.
     * \return true if success else false.
     */
    bool UpdateDatabase();

    /*!
     * \brief UpdateUI - Update the database then  the UI.
     */
    void UpdateUI();

    /*!
     * \brief UpdateUIYear - Update the year without duplicates from the database to the UI.
     */
    void UpdateUIYear();

    /*!
     * \brief UpdateUIModel - Update the model without duplicates from the database to the UI.
     */
    void UpdateUIModel(const QString &selected_year);

    /*!
     * \brief UpdateUIMake - Update the Make without duplicates from the database to the UI.
     */
    void UpdateUIMake(const QString &selected_model);

    Ui::DVUI_LeftPanelVehicleLookup *ui;
    QSqlDatabase vehicle_database_;
    QString file_;
};

#endif // DVHLPVEHICLELOOKUP_H
