@echo off

:: This replaces the system python with the one from Anaconda
CALL C:\ProgramData\Anaconda3\Scripts\activate.bat C:\ProgramData\Anaconda3

:: Install any relevant pip libraries here
pip install pyserial

:: If your batch file is saved in the same folder as your python script, you can use a relative path
python ./GUI_refresh.py

:: You could also use an absolute path like below, but be aware that the drive names may change between computers

:: python C:\PATH\TO\GUI_SCRIPT.py


:: If you want to keep the command prompt window open after the script exits so you can see the error that python
:: printed out, uncomment the next statement

pause