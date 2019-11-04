#include "DVHlpvehiclelookup.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DVHlpvehiclelookup look_up_widget;
    QString csv_file_path=  QCoreApplication::applicationDirPath()+"/TestCase/TrialProject1.csv";
    look_up_widget.SetSourceFile(csv_file_path);
    look_up_widget.Update();
    look_up_widget.show();
    return a.exec();
}
