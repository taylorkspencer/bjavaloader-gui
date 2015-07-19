/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.cc
 * Copyright (C) Taylor Spencer 2010 <>
 * 
 */

#include <gtkmm.h>
#include <iostream>
#include <main.h>
#include <string>
#include <gtkmm/accelmap.h>
using namespace std;

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

/* For testing propose use the local (not installed) ui file */

/* #define UI_FILE PACKAGE_DATA_DIR"/bjavaloader-gui/ui/bjavaloader_gui.ui" */
#define UI_FILE "/usr/share/bjavaloader-gui/bjavaloader_gui.ui"

// Load the Glade file and instiate its widgets:
Glib::RefPtr<Gtk::Builder> builder;
// Declare other strings and widgets that are used globally throughout the program
string currentDevicePIN, currentDeviceName;
bool deviceConnected;

int main(int argc, char *argv[])
{
	Gtk::Main kit(argc, argv);
	try
	{
		builder = Gtk::Builder::create_from_file(UI_FILE);
	}
	catch (const Glib::FileError & ex)
	{
		std::cerr << ex.what() << std::endl;
		return 1;
	}
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	if (!main_win)
	{
		return 1;
	}
	// Create the InstalledModulesListViewText control
	createInstalledModulesListViewText();
	// Begin connecting signals to the builder
	connectSignals();
	kit.signal_run().connect(sigc::ptr_fun(&startupRoutine));
	// Display the main window
	kit.run(*main_win);
	return 0;
}

// Internal method: Create the control InstalledModulesListViewText (which, for some reason,
// cannot be created with Glade) and connects it to InstalledModulesScrollWindow
void createInstalledModulesListViewText()
{
	// Create the InstalledModulesListViewText widget
	Gtk::ListViewText* InstalledModulesListViewText = 0;
	InstalledModulesListViewText = new Gtk::ListViewText(5, false, Gtk::SELECTION_MULTIPLE);
	// Set names of columns for InstalledModulesListViewText
	InstalledModulesListViewText->set_column_title(0, "Module Name");
	InstalledModulesListViewText->set_column_title(1, "???");
	InstalledModulesListViewText->set_column_title(2, "Version");
	InstalledModulesListViewText->set_column_title(3, "???");
	InstalledModulesListViewText->set_column_title(4, "Date Modified");
	// Create interactive search for InstalledModulesListViewText
	InstalledModulesListViewText->set_search_column(0);
	// Add InstalledModulesListViewText to InstalledModulesScrollWindow
	Gtk::ScrolledWindow* InstalledModulesScrollWindow = 0;
	builder->get_widget("InstalledModulesScrollWindow", InstalledModulesScrollWindow);
	InstalledModulesScrollWindow->add(*InstalledModulesListViewText);
	InstalledModulesListViewText->show();
}

// Internal method: Connects the signals to the controls on the window
void connectSignals()
{
	// Signals for toolbar items
	// Signal for rescanButton
	Gtk::Button* rescanButton = 0;
	builder->get_widget("rescanButton", rescanButton);
	rescanButton->signal_clicked().connect(sigc::ptr_fun(&updateDeviceInfo));
	// Signal for reloadEventLogBtn
	Gtk::ToolButton* reloadEventLogBtn = 0;
	builder->get_widget("reloadEventLogBtn", reloadEventLogBtn);
	reloadEventLogBtn->signal_clicked().connect(sigc::ptr_fun(&updateEventLogViewer));
	// Signal for clearEventLogBtn
	Gtk::ToolButton* clearEventLogBtn = 0;
	builder->get_widget("clearEventLogBtn", clearEventLogBtn);
	clearEventLogBtn->signal_clicked().connect(sigc::ptr_fun(&clearEventLog));
	// Signal for addApplicationBtn
	Gtk::ToolButton* addApplicationBtn = 0;
	builder->get_widget("addApplicationBtn", addApplicationBtn);
	addApplicationBtn->signal_clicked().connect(sigc::ptr_fun(&addApplication));
	// Signal for removeApplicationBtn
	Gtk::ToolButton* removeApplicationBtn = 0;
	builder->get_widget("removeApplicationBtn", removeApplicationBtn);
	removeApplicationBtn->signal_clicked().connect(sigc::ptr_fun(&removeApplication));
	// Signal for backupApplicationBtn
	Gtk::ToolButton* backupApplicationBtn = 0;
	builder->get_widget("backupApplicationBtn", backupApplicationBtn);
	backupApplicationBtn->signal_clicked().connect(sigc::ptr_fun(&backupApplication));
	// Signal for reloadApplicationList
	Gtk::ToolButton* reloadApplicationList = 0;
	builder->get_widget("reloadApplicationList", reloadApplicationList);
	reloadApplicationList->signal_clicked().connect(sigc::ptr_fun(&updateInstalledModules));
	// Signals for menu items
	// Signal for mnuFileAddApplication
	Gtk::MenuItem* mnuFileAddApplication = 0;
	builder->get_widget("mnuFileAddApplication", mnuFileAddApplication);
	mnuFileAddApplication->signal_activate().connect(sigc::ptr_fun(&addApplication));
	// Signal for mnuFileRemoveApplication
	Gtk::MenuItem* mnuFileRemoveApplication = 0;
	builder->get_widget("mnuFileRemoveApplication", mnuFileRemoveApplication);
	mnuFileRemoveApplication->signal_activate().connect(sigc::ptr_fun(&removeApplication));
	// Signal for mnuFileBackupApplication
	Gtk::MenuItem* mnuFileBackupApplication = 0;
	builder->get_widget("mnuFileBackupApplication", mnuFileBackupApplication);
	mnuFileBackupApplication->signal_activate().connect(sigc::ptr_fun(&backupApplication));
	// Signal for mnuFileQuit
	Gtk::MenuItem* mnuFileQuit = 0;
	builder->get_widget("mnuFileQuit", mnuFileQuit);
	mnuFileQuit->signal_activate().connect(sigc::ptr_fun(&quit));
	// Signal for mnuEditCopy
	Gtk::MenuItem* mnuEditCopy = 0;
	builder->get_widget("mnuEditCopy", mnuEditCopy);
	mnuEditCopy->signal_activate().connect(sigc::ptr_fun(&copySelection));
	// Signal for mnuEditSelectAll
	Gtk::MenuItem* mnuEditSelectAll = 0;
	builder->get_widget("mnuEditSelectAll", mnuEditSelectAll);
	mnuEditSelectAll->signal_activate().connect(sigc::ptr_fun(&selectAll));
	// Signal for mnuToolsDumpStackTraces
	Gtk::MenuItem* mnuToolsDumpStackTraces = 0;
	builder->get_widget("mnuToolsDumpStackTraces", mnuToolsDumpStackTraces);
	mnuToolsDumpStackTraces->signal_activate().connect(sigc::ptr_fun(&dumpStackTraces));
	// Signal for mnuToolsReloadEventLog
	Gtk::MenuItem* mnuToolsReloadEventLog = 0;
	builder->get_widget("mnuToolsReloadEventLog", mnuToolsReloadEventLog);
	mnuToolsReloadEventLog->signal_activate().connect(sigc::ptr_fun(&updateEventLogViewer));
	// Signal for mnuToolsClearEventLog
	Gtk::MenuItem* mnuToolsClearEventLog = 0;
	builder->get_widget("mnuToolsClearEventLog", mnuToolsClearEventLog);
	mnuToolsClearEventLog->signal_activate().connect(sigc::ptr_fun(&clearEventLog));
	// Signal for mnuToolsTakeScreenshot
	Gtk::MenuItem* mnuToolsTakeScreenshot = 0;
	builder->get_widget("mnuToolsTakeScreenshot", mnuToolsTakeScreenshot);
	mnuToolsTakeScreenshot->signal_activate().connect(sigc::ptr_fun(&takeScreenshot));
	// Signal for mnuToolsPerformFactoryReset
	Gtk::MenuItem* mnuToolsPerformFactoryReset = 0;
	builder->get_widget("mnuToolsPerformFactoryReset", mnuToolsPerformFactoryReset);
	mnuToolsPerformFactoryReset->signal_activate().connect(sigc::ptr_fun(&performFactoryReset));
	// Signal for mnuToolsWipe
	Gtk::MenuItem* mnuToolsWipe = 0;
	builder->get_widget("mnuToolsWipe", mnuToolsWipe);
	mnuToolsWipe->signal_activate().connect(sigc::ptr_fun(&wipeThisBlackBerry));
	// Signal for mnuHelpAboutThisDevice
	Gtk::MenuItem* mnuHelpAboutThisDevice = 0;
	builder->get_widget("mnuHelpAboutThisDevice", mnuHelpAboutThisDevice);
	mnuHelpAboutThisDevice->signal_activate().connect(sigc::ptr_fun(&showAboutThisDeviceDialog));
	// Signal for mnuHelpAbout
	Gtk::MenuItem* mnuHelpAbout = 0;
	builder->get_widget("mnuHelpAbout", mnuHelpAbout);
	mnuHelpAbout->signal_activate().connect(sigc::ptr_fun(&showAboutDialog));
}

// Internal method: Changes global boolean value deviceConnected and enables or
// disables buttons and menu items that require a connected device
void setDeviceConnected(bool state)
{
	// Update global boolean value deviceConnected
	deviceConnected = state;
	// Begin changing states of items
	// Text boxes
	// CurrentlyConnectedDevicesComboBox
	Gtk::ComboBoxEntry* CurrentlyConnectedDevicesComboBox = 0;
	builder->get_widget("CurrentlyConnectedDevicesComboBox", CurrentlyConnectedDevicesComboBox);
	CurrentlyConnectedDevicesComboBox->set_sensitive(state);
	// InstalledModulesListViewText
	Gtk::ScrolledWindow* InstalledModulesScrollWindow = 0;
	builder->get_widget("InstalledModulesScrollWindow", InstalledModulesScrollWindow);
	InstalledModulesScrollWindow->get_child()->set_sensitive(state);
	// eventLogViewer
	Gtk::TextView* eventLogViewer = 0;
	builder->get_widget("eventLogViewer", eventLogViewer);
	eventLogViewer->set_sensitive(state);
	// Items on toolbars
	// reloadEventLogBtn
	Gtk::ToolButton* reloadEventLogBtn = 0;
	builder->get_widget("reloadEventLogBtn", reloadEventLogBtn);
	reloadEventLogBtn->set_sensitive(state);
	// clearEventLogBtn
	Gtk::ToolButton* clearEventLogBtn = 0;
	builder->get_widget("clearEventLogBtn", clearEventLogBtn);
	clearEventLogBtn->set_sensitive(state);
	// addApplicationBtn
	Gtk::ToolButton* addApplicationBtn = 0;
	builder->get_widget("addApplicationBtn", addApplicationBtn);
	addApplicationBtn->set_sensitive(state);
	// removeApplicationBtn
	Gtk::ToolButton* removeApplicationBtn = 0;
	builder->get_widget("removeApplicationBtn", removeApplicationBtn);
	removeApplicationBtn->set_sensitive(state);
	// backupApplicationBtn
	Gtk::ToolButton* backupApplicationBtn = 0;
	builder->get_widget("backupApplicationBtn", backupApplicationBtn);
	backupApplicationBtn->set_sensitive(state);
	// Signal for reloadApplicationList
	Gtk::ToolButton* reloadApplicationList = 0;
	builder->get_widget("reloadApplicationList", reloadApplicationList);
	reloadApplicationList->set_sensitive(state);
	// Items on menus
	// mnuFileAddApplication
	Gtk::MenuItem* mnuFileAddApplication = 0;
	builder->get_widget("mnuFileAddApplication", mnuFileAddApplication);
	mnuFileAddApplication->set_sensitive(state);
	// mnuFileRemoveApplication
	Gtk::MenuItem* mnuFileRemoveApplication = 0;
	builder->get_widget("mnuFileRemoveApplication", mnuFileRemoveApplication);
	mnuFileRemoveApplication->set_sensitive(state);
	// mnuFileBackupApplication
	Gtk::MenuItem* mnuFileBackupApplication = 0;
	builder->get_widget("mnuFileBackupApplication", mnuFileBackupApplication);
	mnuFileBackupApplication->set_sensitive(state);
	// mnuEditCopy
	Gtk::MenuItem* mnuEditCopy = 0;
	builder->get_widget("mnuEditCopy", mnuEditCopy);
	mnuEditCopy->set_sensitive(state);
	// mnuEditSelectAll
	Gtk::MenuItem* mnuEditSelectAll = 0;
	builder->get_widget("mnuEditSelectAll", mnuEditSelectAll);
	mnuEditSelectAll->set_sensitive(state);
	// mnuToolsDumpStackTraces
	Gtk::MenuItem* mnuToolsDumpStackTraces = 0;
	builder->get_widget("mnuToolsDumpStackTraces", mnuToolsDumpStackTraces);
	mnuToolsDumpStackTraces->set_sensitive(state);
	// mnuToolsReloadEventLog
	Gtk::MenuItem* mnuToolsReloadEventLog = 0;
	builder->get_widget("mnuToolsReloadEventLog", mnuToolsReloadEventLog);
	mnuToolsReloadEventLog->set_sensitive(state);
	// mnuToolsClearEventLog
	Gtk::MenuItem* mnuToolsClearEventLog = 0;
	builder->get_widget("mnuToolsClearEventLog", mnuToolsClearEventLog);
	mnuToolsClearEventLog->set_sensitive(state);
	// mnuToolsTakeScreenshot
	Gtk::MenuItem* mnuToolsTakeScreenshot = 0;
	builder->get_widget("mnuToolsTakeScreenshot", mnuToolsTakeScreenshot);
	mnuToolsTakeScreenshot->set_sensitive(state);
	// mnuToolsPerformFactoryReset
	Gtk::MenuItem* mnuToolsPerformFactoryReset = 0;
	builder->get_widget("mnuToolsPerformFactoryReset", mnuToolsPerformFactoryReset);
	mnuToolsPerformFactoryReset->set_sensitive(state);
	// mnuToolsWipe
	Gtk::MenuItem* mnuToolsWipe = 0;
	builder->get_widget("mnuToolsWipe", mnuToolsWipe);
	mnuToolsWipe->set_sensitive(state);
	// mnuHelpAboutThisDevice
	Gtk::MenuItem* mnuHelpAboutThisDevice = 0;
	builder->get_widget("mnuHelpAboutThisDevice", mnuHelpAboutThisDevice);
	mnuHelpAboutThisDevice->set_sensitive(state);
}

// Internal method: Sets the text of the status bar to the string text
void setStatusBarText(string text)
{
	Gtk::Label* statusBarText = 0;
	builder->get_widget("statusBarText", statusBarText);
	if (text=="") // If the text is blank, hide the status bar text
	{
		statusBarText->set_visible(false);
	}
	else
	{
		statusBarText->set_visible(true);
		statusBarText->set_label(text);
	}
	// Force a redraw with the new text
	statusBarText->queue_draw();
	statusBarText->get_window()->process_updates(true);
}

// Internal method: Sets the progress of the status bar to the float progress
void setStatusBarProgress(float progress)
{
	Gtk::ProgressBar* statusBarProgress = 0;
	builder->get_widget("statusBarProgress", statusBarProgress);
	if (progress==-1) // -1 is a signal to turn off the progress bar
	{
		statusBarProgress->set_visible(false);
	}
	else
	{
		statusBarProgress->set_visible(true);
		statusBarProgress->set_fraction(progress);
	}
	// Force a redraw with the new percentage
	statusBarProgress->queue_draw();
	statusBarProgress->get_window()->process_updates(true);
}

// Interal method: Runs at the startup of the program
void startupRoutine()
{
	setStatusBarProgress(.25);
	updateDeviceInfo();
	setStatusBarProgress(.50);
	updateInstalledModules();
	setStatusBarProgress(.75);
	updateEventLogViewer();
	onDone();
}

// Internal method: ran whenever a task that freezes the window finishes
void onDone()
{
	setStatusBarText("");
	setStatusBarProgress(-1);
}

// Displays an About dialog
void showAboutDialog()
{
	// Prepare the About dialog
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	Gtk::AboutDialog aboutDialog;
	aboutDialog.set_program_name("Bjavaloader GUI");
	aboutDialog.set_version("1.0");
	aboutDialog.set_copyright("Taylor Spencer");
	// Display the Save Screenshot dialog and extract the selection
	int response = aboutDialog.run();
}

// Self-explaintory: exits the program
void quit()
{
	exit(0);
}

// Companion method to updateDeviceInfo(): Detects if a BlackBerry device is connected and gathers info about it
string getBasicDeviceInfo()
{
	char *toReturn;
	// Update the window display before it locks up
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	main_win->queue_draw();
	// To catch an error caused by btool not being installed
	if (!g_spawn_command_line_sync("btool", &toReturn, NULL, NULL, NULL))
	{
		return NULL; // Return a null string to indicate to the parent process that an error has occured
	}
	return string(toReturn);
}

// Companion method to showAboutThisDeviceDialog()
string getAdvancedDeviceInfo()
{
	char *toReturn;
	// To catch an error caused by bjavaloader not being installed
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" deviceinfo").c_str(), &toReturn, NULL, NULL, NULL))
	{
		return NULL; // Return a null string to indicate to the parent process that an error has occured
	}
	return string(toReturn);
}

// Detects if a device is connected and updates the name and PIN number of the device connected (if any)
void updateDeviceInfo()
{
	setStatusBarText("Updating device info...");
	string DeviceInfoString = getBasicDeviceInfo();
	if (DeviceInfoString.empty())
	{
		// Print a message to the status bar
		setStatusBarText("Btool is either not installed or is not registered with the shell.  Please install btool and try again.");
		setDeviceConnected(false);
		return;
	}
	else if (DeviceInfoString=="Blackberry devices found:\n")
	{
		// Update CurrentlyConnectedDevicesComboBox to indicate that there are no devices
		Gtk::ComboBoxEntry* CurrentlyConnectedDevicesComboBox = 0;
		builder->get_widget("CurrentlyConnectedDevicesComboBox", CurrentlyConnectedDevicesComboBox);
		CurrentlyConnectedDevicesComboBox->get_entry()->set_text("No devices found");
		// Disable items pointing to actions that require a device to perform
		setDeviceConnected(false);
		return;
	}
	// Clear previous contents from currentDevicePIN
	currentDevicePIN.clear();
	// Extract device PIN number from DeviceInfoString and put it in currentDevicePIN
	currentDevicePIN = DeviceInfoString.substr((DeviceInfoString.find("PIN:")+5), 8);
	// Clear previous contents from currentDeviceName
	currentDeviceName.clear();
	// Extract device name from DeviceInfoString and put it in currentDeviceName
	currentDeviceName = DeviceInfoString.substr((DeviceInfoString.find("Name:")+6), ((DeviceInfoString.length()-30)-(DeviceInfoString.find("Name:")+6)));
	// Update CurrentlyConnectedDevicesComboBox with the name of the currently connected device
	Gtk::ComboBoxEntry* CurrentlyConnectedDevicesComboBox = 0;
	builder->get_widget("CurrentlyConnectedDevicesComboBox", CurrentlyConnectedDevicesComboBox);
	// Enable items pointing to actions that require a device to perform
	setDeviceConnected(true);
	CurrentlyConnectedDevicesComboBox->get_entry()->set_text(currentDeviceName+" (PIN: "+currentDevicePIN+")");
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Companion method to updateInstalledModules(): Collects a list of installed modules
string getRawInstalledModules()
{
	char *toReturn;
	// To catch an error caused by bjavaloader not being installed
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" dir").c_str(), &toReturn, NULL, NULL, NULL))
	{
		return NULL; // Return a null string to indicate to the parent process that an error has occured
	}
	return string(toReturn);
}

// Gets the list of installed modules and puts them in InstalledModulesListViewText
void updateInstalledModules()
{
	setStatusBarText("Updating installed modules...");
	// If no device is connected, exit this method
	if (!deviceConnected)
	{
		return;
	}
	string InstalledModulesString = getRawInstalledModules();
	if (InstalledModulesString.empty())
	{
		return;
	}
	// Set up the istringstream used to split InstalledModulesString into lines
	istringstream stringParser;
	stringParser.str(InstalledModulesString);
	string firstLineString;
	getline(stringParser, firstLineString);
	// Get the number of lines in InstalledModulesString
	string numberOfLinesString = firstLineString.substr((firstLineString.find("Directory:")+11),
	                                                    ((firstLineString.find("Directory:")+11)-firstLineString.find("/")));
	int numberOfLines=((atoi(numberOfLinesString.c_str())*2)+1);
	Gtk::ScrolledWindow* InstalledModulesScrollWindow = 0;
	builder->get_widget("InstalledModulesScrollWindow", InstalledModulesScrollWindow);
	Gtk::ListViewText* InstalledModulesListViewText = (Gtk::ListViewText*)InstalledModulesScrollWindow->get_child();
	// Clear out the contents from any previous runs
	InstalledModulesListViewText->clear_items();
	// Capture the module names and information (found on every other row)
	string rawModuleName, moduleName, rawModuleProperties;
	int modulePropertiesStartLocations[4];
	int modulePropertiesEndLocations[4];
	string moduleProperties[4];
	for (int n=0; n<(numberOfLines/2); n++)
	{
		// Add the name of the module to the first column from the first line
		InstalledModulesListViewText->append_text("");
		rawModuleName = "";
		getline(stringParser, rawModuleName);
		// Remove extra spaces from the module name before adding to column
		moduleName = rawModuleName.substr(rawModuleName.find_first_not_of(' '),
		                                  (rawModuleName.find_last_not_of(' ')-rawModuleName.find_first_not_of(' ')+1));
		InstalledModulesListViewText->set_text(n, 0, moduleName);
		// Parse the second line and use it to fill the other columns
		rawModuleProperties = "";
		getline(stringParser, rawModuleProperties);
		// Collect the locations of the start and finish of the module properties
		modulePropertiesStartLocations[0] = rawModuleProperties.find_first_not_of(' ');
		modulePropertiesEndLocations[0] = rawModuleProperties.find(' ', modulePropertiesStartLocations[0]);
		modulePropertiesStartLocations[1] = rawModuleProperties.find_first_not_of(' ', modulePropertiesEndLocations[0]);
		modulePropertiesEndLocations[1] = rawModuleProperties.find(' ', modulePropertiesStartLocations[1]);
		modulePropertiesStartLocations[2] = rawModuleProperties.find_first_not_of(' ', modulePropertiesEndLocations[1]);
		modulePropertiesEndLocations[2] = rawModuleProperties.find(' ', modulePropertiesStartLocations[2]);
		modulePropertiesStartLocations[3] = rawModuleProperties.find_first_not_of(' ', modulePropertiesEndLocations[2]);
		modulePropertiesEndLocations[3] = (rawModuleProperties.find_last_not_of(' ')+1);
		// Use these locations to create strings of the module properties
		moduleProperties[0] = rawModuleProperties.substr(modulePropertiesStartLocations[0], (modulePropertiesEndLocations[0]-modulePropertiesStartLocations[0]));
		moduleProperties[1] = rawModuleProperties.substr(modulePropertiesStartLocations[1], (modulePropertiesEndLocations[1]-modulePropertiesStartLocations[1]));
		moduleProperties[2] = rawModuleProperties.substr(modulePropertiesStartLocations[2], (modulePropertiesEndLocations[2]-modulePropertiesStartLocations[2]));
		moduleProperties[3] = rawModuleProperties.substr(modulePropertiesStartLocations[3], (modulePropertiesEndLocations[3]-modulePropertiesStartLocations[3]));
		// Fill the columns with the strings of the module properties
		InstalledModulesListViewText->set_text(n, 1, moduleProperties[0]);
		InstalledModulesListViewText->set_text(n, 2, moduleProperties[1]);
		InstalledModulesListViewText->set_text(n, 3, moduleProperties[2]);
		InstalledModulesListViewText->set_text(n, 4, moduleProperties[3]);
	}
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Companion method to updateEventLogViewer(): Collects the device's event log
string getRawEventLog()
{
	char *toReturn;
	// To catch an error caused by bjavaloader not being installed
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" eventlog").c_str(), &toReturn, NULL, NULL, NULL))
	{
		return NULL; // Return a null string to indicate to the parent process that an error has occured
	}
	return string(toReturn);
}

// Loads the BlackBerry event log and puts it in EventLogViewer
void updateEventLogViewer()
{
	setStatusBarText("Updating the event log...");
	// If no device is connected, exit this method
	if (!deviceConnected)
	{
		return;
	}
	string EventLogString = getRawEventLog();
	Gtk::TextView* eventLogViewer = 0;
	builder->get_widget("eventLogViewer", eventLogViewer);
	eventLogViewer->get_buffer()->set_text(EventLogString);
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Dumps all the stack traces into the event log and updates EventLogViewer
void dumpStackTraces()
{
	// Tell bjavaloader to dump the stack traces into the event log
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" logstacktraces").c_str(), NULL, NULL, NULL, NULL))
	{
		return;
	}
	// Update the event log
	updateEventLogViewer();
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Clears the device event log and updates EventLogViewer
void clearEventLog()
{
	// Tell bjavaloader to clear the event log
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" cleareventlog").c_str(), NULL, NULL, NULL, NULL))
	{
		return;
	}
	// Update the event log
	updateEventLogViewer();
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Triggers the GTK open file dialog box and adds the application to the BlackBerry
void addApplication()
{
	// Prepare the Add Application dialog
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	Gtk::FileChooserDialog addApplicationDialog(*main_win, "Add Application", Gtk::FILE_CHOOSER_ACTION_OPEN);
	addApplicationDialog.set_filename("*.cod");
	addApplicationDialog.set_select_multiple(true);
	addApplicationDialog.set_local_only(true); // Necessary because bjavaloader doesn't accept file urls
	addApplicationDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	addApplicationDialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);
	// Display the Add Application dialog and extract the selection
	int response = addApplicationDialog.run();
	// In case the user pressed the Cancel button or closed the dialog
	if ((response == Gtk::RESPONSE_CANCEL)||(response == Gtk::RESPONSE_CLOSE))
	{
		return;
	}
	// Determine which items are selected and put them into a string
	string rawApplicationToAdd, applicationsToAdd;
	vector<Glib::ustring> rawApplicationsToAddVector;
	rawApplicationsToAddVector = addApplicationDialog.get_uris();
	for (int n=0; n<=(addApplicationDialog.get_uris().size()-1); n++)
	{
		rawApplicationToAdd = rawApplicationsToAddVector[n];
		// Again, this is necessary because bjavaloader doesn't accept file urls
		applicationsToAdd+=rawApplicationToAdd.substr(7, (rawApplicationToAdd.length()-7));
		// If this is not the last item, add a space to the string
		if (n!=(addApplicationDialog.get_uris().size()-1))
		{
			applicationsToAdd+=" ";
		}
	}
	// Tell bjavaloader to add the application(s) to the device
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" load "+applicationsToAdd).c_str(), NULL, NULL, NULL, NULL))
	{
		return;
	}
	// Update the list of installed applications
	updateInstalledModules();
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Remove the application(s) selected in InstalledModulesListViewText
void removeApplication()
{
	// Get the first column of the selection(s) of InstalledModulesListViewText
	Gtk::ScrolledWindow* InstalledModulesScrollWindow = 0;
	builder->get_widget("InstalledModulesScrollWindow", InstalledModulesScrollWindow);
	Gtk::ListViewText* InstalledModulesListViewText = (Gtk::ListViewText*)InstalledModulesScrollWindow->get_child();
	// Determine which items are selected and put them into a string
	string applicationsToRemove;
	for (int n=0; n<=(InstalledModulesListViewText->get_selection()->count_selected_rows()-1); n++)
	{
		applicationsToRemove+=InstalledModulesListViewText->get_text(InstalledModulesListViewText->get_selected()[n], 0);
		// If this is not the last item, add a space to the string
		if (n!=(InstalledModulesListViewText->get_selection()->count_selected_rows()-1))
		{
			applicationsToRemove+=" ";
		}
	}
	// Tell bjavaloader to remove the application(s) from the device
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" erase "+applicationsToRemove).c_str(), NULL, NULL, NULL, NULL))
	{
		return;
	}
	// Update the list of installed applications
	updateInstalledModules();
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Backs up the application(s) selected in InstalledModulesListViewText to a location selected by the user
void backupApplication()
{
	// Derive the names of the selected modules from InstalledModulesListViewText
	Gtk::ScrolledWindow* InstalledModulesScrollWindow = 0;
	builder->get_widget("InstalledModulesScrollWindow", InstalledModulesScrollWindow);
	Gtk::ListViewText* InstalledModulesListViewText = (Gtk::ListViewText*)InstalledModulesScrollWindow->get_child();
	// Determine which items are selected and put them into a string
	string applicationsToBackup;
	for (int n=0; n<=(InstalledModulesListViewText->get_selection()->count_selected_rows()-1); n++)
	{
		applicationsToBackup+=InstalledModulesListViewText->get_text(InstalledModulesListViewText->get_selected()[n], 0);
		// If this is not the last item, add a space to the string
		if (n!=(InstalledModulesListViewText->get_selection()->count_selected_rows()-1))
		{
			applicationsToBackup+=" ";
		}
	}
	// Prepare the Backup Application dialog
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	Gtk::FileChooserDialog backupApplicationDialog(*main_win, "Backup Application", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
	backupApplicationDialog.set_select_multiple(false);
	backupApplicationDialog.set_local_only(true); // Necessary because bjavaloader doesn't accept file urls
	backupApplicationDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	backupApplicationDialog.add_button("_Backup Here", Gtk::RESPONSE_ACCEPT);
	// Display the Backup Application dialog and extract the selection
	int response = backupApplicationDialog.run();
	// In case the user pressed the Cancel button or closed the dialog
	if ((response == Gtk::RESPONSE_CANCEL)||(response == Gtk::RESPONSE_CLOSE))
	{
		return;
	}
	string rawBackupDirectory = backupApplicationDialog.get_uri();
	// Again, this is necessary because bjavaloader doesn't accept file urls
	string backupDirectory = rawBackupDirectory.substr(7, (rawBackupDirectory.length()-7));
	while (backupDirectory.find("%20")!=-1)
	{
		backupDirectory.replace(backupDirectory.find("%20"), 3, "\\ ");
	}
	// Tell bjavaloader to back up the application(s) to the directory specified
	// I'm using popen here because g_spawn_command_line_sync does not accept more than one command at a time
	FILE *bjavaloader = popen(("cd "+backupDirectory+" && bjavaloader -p "+currentDevicePIN+" save "+applicationsToBackup).c_str(), "r");
	pclose(bjavaloader);
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Takes a screenshot of the BlackBerry screen and saves it to a directory specified by the user
void takeScreenshot()
{
	// Prepare the Save Screenshot dialog
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	Gtk::FileChooserDialog saveScreenshotDialog(*main_win, "Save Screenshot", Gtk::FILE_CHOOSER_ACTION_SAVE);
	saveScreenshotDialog.set_select_multiple(false);
	saveScreenshotDialog.set_local_only(true); // Necessary because bjavaloader doesn't accept file urls
	saveScreenshotDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	saveScreenshotDialog.add_button("_Save", Gtk::RESPONSE_ACCEPT);
	// Display the Save Screenshot dialog and extract the selection
	int response = saveScreenshotDialog.run();
	// In case the user pressed the Cancel button or closed the dialog
	if ((response == Gtk::RESPONSE_CANCEL)||(response == Gtk::RESPONSE_CLOSE))
	{
		return;
	}
	string rawScreenshotFile = saveScreenshotDialog.get_uri();
	// Again, this is necessary because bjavaloader doesn't accept file urls
	string screenshotFile = rawScreenshotFile.substr(7, (rawScreenshotFile.length()-7));
	if (!g_spawn_command_line_sync(("bjavaloader -p "+currentDevicePIN+" screenshot "+screenshotFile).c_str(), NULL, NULL, NULL, NULL))
	{
		return;
	}
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Performs a factory reset to the device
void performFactoryReset()
{
	// Prepare the Perform Factory Reset dialog
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	Gtk::MessageDialog performFactoryResetDialog(*main_win, "Warning: You are about to perform a factory reset to your BlackBerry.",
	                                            false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_OK_CANCEL);
	performFactoryResetDialog.set_title("Perform Factory Reset");
	performFactoryResetDialog.set_secondary_text("This will remove your data and any after-market installed applications.  It may also downgrade your OS, however, this is unlikely.  Proceed with caution.",
	                                            false);
	// Display the Perform Factory Reset dialog and extract the selection
	int response = performFactoryResetDialog.run();
	// In case the user pressed the Cancel button or closed the dialog
	if ((response == Gtk::RESPONSE_CANCEL)||(response == Gtk::RESPONSE_CLOSE))
	{
		return;
	}
	// Perform a factory reset to the device
	FILE *bjavaloader = popen(("bjavaloader -p "+currentDevicePIN+" resettofactory").c_str(), "w");
	fwrite("yes", 1, 3, bjavaloader); // Automatically fill in the prompt when prompted
	pclose(bjavaloader); // Don't go to the next routine until this operation is finished
	// Wait for device to reboot and become reconnected
	setDeviceConnected(false);
	while (!deviceConnected)
	{
		updateDeviceInfo();
	}
	// Update the InstalledModulesListViewText and eventLogViewer
	updateInstalledModules();
	updateEventLogViewer();
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Allows the user to wipe the data and/or applications off their BlackBerry
void wipeThisBlackBerry()
{
	// Declare custom responses for the Wipe this BlackBerry dialog
	const int RESPONSE_WIPE_DATA_ONLY=6;
	const int RESPONSE_WIPE_APPLICATIONS_ONLY=7;
	// Prepare the Wipe this BlackBerry dialog
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	Gtk::MessageDialog wipeThisBlackBerryDialog(*main_win, "Warning: You are about to wipe your BlackBerry.",
	                                            false, Gtk::MESSAGE_WARNING, Gtk::BUTTONS_NONE);
	wipeThisBlackBerryDialog.set_title("Wipe this BlackBerry");
	wipeThisBlackBerryDialog.set_secondary_text("This will remove everything from your BlackBerry, including your OS, applications, contacts, appointments, and configuration settings.  You will need to reload your OS using another program after performing this operation to return to a working BlackBerry.\n\nIf you wish to remove your data but preserve your applications and OS, choose \"Wipe Data Only\".  If you wish to preserve your data but wipe your applications and OS, choose \"Wipe Applications Only\".  If you wish to prepare your BlackBerry for resale or disposal, press \"Cancel\" and choose \"Factory Reset\" from the Tools menu.  Unless you choose \"Wipe Data Only\", this WILL brick your BlackBerry.  Proceed with caution.",
	                                            false);
	wipeThisBlackBerryDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
	wipeThisBlackBerryDialog.add_button("Wipe _Data Only", RESPONSE_WIPE_DATA_ONLY);
	wipeThisBlackBerryDialog.add_button("Wipe _Applications Only", RESPONSE_WIPE_APPLICATIONS_ONLY);
	wipeThisBlackBerryDialog.add_button("Wipe _Everything", Gtk::RESPONSE_ACCEPT);
	// Display the Wipe this BlackBerry dialog and extract the selection
	int response = wipeThisBlackBerryDialog.run();
	// In case the user pressed the Cancel button or closed the dialog
	if ((response == Gtk::RESPONSE_CANCEL)||(response == Gtk::RESPONSE_CLOSE))
	{
		return;
	}
	// If the user chose to wipe data only
	else if (response == RESPONSE_WIPE_DATA_ONLY)
	{
		FILE *bjavaloader = popen(("bjavaloader -p "+currentDevicePIN+" wipe -i").c_str(), "w");
		fwrite("yes", 1, 3, bjavaloader); // Automatically fill in the prompt when prompted
		pclose(bjavaloader); // Don't go to the next routine until this operation is finished
	}
	// If the user chose to wipe applications only
	else if (response == RESPONSE_WIPE_APPLICATIONS_ONLY)
	{
		FILE *bjavaloader = popen(("bjavaloader -p "+currentDevicePIN+" wipe -a").c_str(), "w");
		fwrite("yes", 1, 3, bjavaloader); // Automatically fill in the prompt when prompted
		pclose(bjavaloader); // Don't go to the next routine until this operation is finished
	}
	// If the user chose to wipe data and applications
	else if (response == Gtk::RESPONSE_ACCEPT)
	{
		FILE *bjavaloader = popen(("bjavaloader -p "+currentDevicePIN+" wipe").c_str(), "w");
		fwrite("yes", 1, 3, bjavaloader); // Automatically fill in the prompt when prompted
		pclose(bjavaloader); // Don't go to the next routine until this operation is finished
	}
	if (!Gtk::Main::events_pending())
	{
		onDone();
	}
}

// Displays a dialog containing various information about their BlackBerry, including model and OS version
void showAboutThisDeviceDialog()
{
	// Prepare the About this Device dialog
	Gtk::Window* main_win = 0;
	builder->get_widget("main_window", main_win);
	Gtk::MessageDialog aboutThisDeviceDialog(*main_win, currentDeviceName.c_str(), false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
	aboutThisDeviceDialog.set_title("About this Device");
	// Collect more detailed info about the connected device
	string AdvancedDeviceInfoString = getAdvancedDeviceInfo();
	// Extract device OS version from AdvancedDeviceInfoString and put it in deviceOSVersion
	string deviceOSVersion = AdvancedDeviceInfoString.substr((AdvancedDeviceInfoString.find("VM Version:")+17), 9);
	// Extract device firmware version from AdvancedDeviceInfoString and put it in deviceFirmwareVersion
	string deviceFirmwareVersion = AdvancedDeviceInfoString.substr((AdvancedDeviceInfoString.find("OS Version:")+17), 7);
	// Extract device hardware ID from AdvancedDeviceInfoString and put it in deviceHardwareID
	string deviceHardwareID = AdvancedDeviceInfoString.substr((AdvancedDeviceInfoString.find("Hardware Id:")+17), 10);
	// Extract device radio ID from AdvancedDeviceInfoString and put it in deviceRadioID
	string deviceRadioID = AdvancedDeviceInfoString.substr((AdvancedDeviceInfoString.find("Radio ID:")+17), 3);
	// Extract device vendor ID from AdvancedDeviceInfoString and put it in deviceVendorID
	string deviceVendorID = AdvancedDeviceInfoString.substr((AdvancedDeviceInfoString.find("Vendor ID:")+17), 3);
	// Extract device active WAFs from AdvancedDeviceInfoString and put it in deviceActiveWAFs
	string deviceActiveWAFs = AdvancedDeviceInfoString.substr((AdvancedDeviceInfoString.find("Active WAFs:")+17), 3);
	aboutThisDeviceDialog.set_secondary_text(("PIN Number:\t\t\t"+currentDevicePIN+"\nOS Version:\t\t\t"+deviceOSVersion+"\nFirmware Version:\t"+deviceFirmwareVersion+"\nHardware ID:\t\t\t"+deviceHardwareID+"\nRadio ID:\t\t\t\t"+deviceRadioID+"\nVendor ID:\t\t\t"+deviceVendorID+"\nActive WAFs:\t\t\t"+deviceActiveWAFs+"").c_str(),
	                                            false);
	// Display the About this Device dialog
	aboutThisDeviceDialog.run();
}

// Copies the selected item or text to the clipboard
void copySelection()
{
	// Determine which tab has the focus
	Gtk::Notebook* notebook1 = 0;
	builder->get_widget("notebook1", notebook1);
	// If the Applications tab is selected
	if (notebook1->get_current_page()==0)
	{
		// Get the selected text from InstalledModulesListViewText and copy it to the clipboard
		Gtk::ScrolledWindow* InstalledModulesScrollWindow = 0;
		builder->get_widget("InstalledModulesScrollWindow", InstalledModulesScrollWindow);
		Gtk::ListViewText* InstalledModulesListViewText = (Gtk::ListViewText*)InstalledModulesScrollWindow->get_child();
		string selectedRowsContents;
		for (int n=0; n<=(InstalledModulesListViewText->get_selection()->count_selected_rows()-1); n++)
		{
			selectedRowsContents+=(InstalledModulesListViewText->get_text(InstalledModulesListViewText->get_selected()[n], 0)+'\t');
			selectedRowsContents+=(InstalledModulesListViewText->get_text(InstalledModulesListViewText->get_selected()[n], 1)+'\t');
			selectedRowsContents+=(InstalledModulesListViewText->get_text(InstalledModulesListViewText->get_selected()[n], 2)+'\t');
			selectedRowsContents+=(InstalledModulesListViewText->get_text(InstalledModulesListViewText->get_selected()[n], 3)+'\t');
			selectedRowsContents+=(InstalledModulesListViewText->get_text(InstalledModulesListViewText->get_selected()[n], 4)+'\n');
		}
		Gtk::Clipboard::get(GDK_SELECTION_CLIPBOARD)->set_text(selectedRowsContents);
	}
	// If the Event Logs tab is selected
	else if (notebook1->get_current_page()==1)
	{
		// Get the selected text from eventLogViewer and copy it to the clipboard
		Gtk::TextView* eventLogViewer = 0;
		builder->get_widget("eventLogViewer", eventLogViewer);
		eventLogViewer->get_buffer()->copy_clipboard(Gtk::Clipboard::get(GDK_SELECTION_CLIPBOARD));
	}
}

// Selects all the contents of the item that has focus
void selectAll()
{
	// Determine which tab has the focus
	Gtk::Notebook* notebook1 = 0;
	builder->get_widget("notebook1", notebook1);
	// If the Applications tab is selected
	if (notebook1->get_current_page()==0)
	{
		// Select all the items in InstalledModulesListViewText
		Gtk::ScrolledWindow* InstalledModulesScrollWindow = 0;
		builder->get_widget("InstalledModulesScrollWindow", InstalledModulesScrollWindow);
		Gtk::ListViewText* InstalledModulesListViewText = (Gtk::ListViewText*)InstalledModulesScrollWindow->get_child();
		InstalledModulesListViewText->get_selection()->select_all();	
	}
	// If the Event Logs tab is selected
	else if (notebook1->get_current_page()==1)
	{
		// Select all the text in eventLogViewer
		Gtk::TextView* eventLogViewer = 0;
		builder->get_widget("eventLogViewer", eventLogViewer);
		eventLogViewer->get_buffer()->select_range(eventLogViewer->get_buffer()->begin(), eventLogViewer->get_buffer()->end());
	}
}