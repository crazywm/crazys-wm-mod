#include "DirPath.h"
#include "cInterfaceWindow.h"
#include "InterfaceGlobals.h"

class cScreenMainMenu : public cInterfaceWindowXML
{

public:
	
	static	bool		ids_set; 
	
	cScreenMainMenu()
	{
		cConfig cfg;
		DirPath dp = DirPath()
			<< "Resources"
			<< "Interface"
			<< cfg.resolution.resolution()
			<< "main_menu.xml"
			;
		m_filename = dp.c_str();
	}


	int new_id;
	int load_id;
	int quit_id;

	void set_ids();

	void init();
	void process();
	void check_events();
	bool check_keys();

	void Free() { cInterfaceWindow::Free(); }
};