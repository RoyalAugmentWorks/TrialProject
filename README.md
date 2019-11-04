# TrialProject
Repository for the trial projects.

Use TestBinary:-
1) Simply go to the TestBinary folder and Execute VehicleInfo.exe file to see the working of the GUI.


To Use the Vehicle Look up widget.
Simply follow this steps:-
1) Create an object of the class.
   DVHlpvehiclelookup look_up_widget;
   
2) Set the CSV File Path to read the Vehicle Information.   
  look_up_widget.SetSourceFile(csv_file_path); 
  
3) Call Update Function. This will Update the GUI with the CSV read info .
   look_up_widget.Update();
   
4) Show the GUI .
   look_up_widget.show();
