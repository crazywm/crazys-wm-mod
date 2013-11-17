#include "cInterfaceWindow.h"

class cScreenBrothelManagement : public cInterfaceWindowXML
{
	void set_ids();
	
	static bool id_set;

	int id_header;
	int id_details;
	int id_image;
	int id_girls;
	int id_staff;
	int id_setup;
	int id_dungeon;
	int id_town;
	int id_turn;
	int id_week;
	int id_save;
	int id_quit;
	int id_prev;
	int id_next;

public:

	cScreenBrothelManagement();
	~cScreenBrothelManagement();

	void init();
	void process();
	void check_events();
	bool check_keys();

	void Free() { cInterfaceWindowXML::Free(); }
};

