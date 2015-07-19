// Internal method: Create the control InstalledModulesListViewText (which, for some reason,
// cannot be created with Glade) and connects it to InstalledModulesScrollWindow
void createInstalledModulesListViewText();
// Internal method: Connects the signals to the controls on the window
void connectSignals();
// Internal method: Changes global boolean value deviceConnected and enables or
// disables buttons and menu items that require a connected device
void setDeviceConnected(bool state);
// Internal method: Sets the text of the status bar to the string text
void setStatusBarText(std::string text);
// Internal method: Sets the progress of the status bar to the float progress
void setStatusBarProgress(float progress);
// Interal method: Runs at the startup of the program
void startupRoutine();
// Internal method: ran whenever a task that freezes the window finishes
void onDone();
// Displays an About dialog
void showAboutDialog();
// Self-explaintory: exits the program
void quit();
// Companion method to updateDeviceInfo(): Detects if a BlackBerry device is connected and gathers info about it
std::string getBasicDeviceInfo();
// Companion method to showAboutThisDeviceDialog()
std::string getAdvancedDeviceInfo();
// Detects if a device is connected and updates the name and PIN number of the device connected (if any)
void updateDeviceInfo();
// Companion method to updateInstalledModules(): Collects a list of installed modules
std::string getRawInstalledModules();
// Gets the list of installed modules and puts them in InstalledModulesTreeView
void updateInstalledModules();
// Companion method to updateEventLogViewer(): Collects the device's event log
std::string getRawEventLog();
// Loads the BlackBerry event log and puts it in EventLogViewer
void updateEventLogViewer();
// Dumps all the stack traces into the event log and updates EventLogViewer
void dumpStackTraces();
// Clears the device event log and updates EventLogViewer
void clearEventLog();
// Triggers the GTK open file dialog box and adds the application to the BlackBerry
void addApplication();
// Remove the application(s) selected in InstalledModulesTreeView
void removeApplication();
// Backs up the application(s) selected in InstalledModulesTreeView to a location selected by the user
void backupApplication();
// Takes a screenshot of the BlackBerry screen and saves it to a directory specified by the user
void takeScreenshot();
// Performs a factory reset to the device
void performFactoryReset();
// Allows the user to wipe the data and/or applications off their BlackBerry
void wipeThisBlackBerry();
// Displays a dialog containing various information about their BlackBerry, including model and OS version
void showAboutThisDeviceDialog();
// Copies the selected item or text to the clipboard
void copySelection();
// Selects all the contents of the item that has focus
void selectAll();