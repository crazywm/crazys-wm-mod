#include "cInterfaceWindow.h"

enum INPUT_MODE {
	MODE_STRING,
	MODE_INT,
	MODE_CONFIRM,
	MODE_CONFIRM_EXIT,
};

class cScreenGetInput : public cInterfaceWindowXML
{
	bool ids_set = false;

	INPUT_MODE m_profile = MODE_INT; // GetInt, GetString or GetConfirm?

	int id_btn_ok;
	int id_btn_cancel;
	int id_label;
	int id_textfield;

public:

	cScreenGetInput();
	~cScreenGetInput();

	void set_ids();
	void init();
	void process();
	void check_events();
	bool check_keys();

	void Free() { cInterfaceWindowXML::Free(); }

	void ModeGetString() { m_profile = MODE_STRING; };
	void ModeGetInt() { m_profile = MODE_INT; };
	void ModeConfirm() { m_profile = MODE_CONFIRM; };

	void ModeConfirmExit() { m_profile = MODE_CONFIRM_EXIT; };
	void CheckEvents_ConfirmExit();
};

