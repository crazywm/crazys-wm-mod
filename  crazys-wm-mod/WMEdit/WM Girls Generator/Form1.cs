using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Text.RegularExpressions;
using System.Xml;


namespace WM_Girls_Generator
{
    public partial class Form1 : Form
    {
        ArrayList aTraits = new ArrayList();				//ArrayList to store trait info (experiment for trait tooltips), apparently it worked
        DataTable TraitCollection = new DataTable();		//DataTable to store traits in
        DataTable TraitTypes = new DataTable();		        //DataTable to store trait types in
        DataTable iTable = new DataTable();					//DataTable for items effects
        DataTable iTTable = new DataTable();				//DataTable for item traits
        DataTable ItemsCollection = new DataTable();		//DataTable to store items in
        DataTable GirlsCollection = new DataTable();		//DataTable to store girls in
        DataTable GirlsCollectionTemp = new DataTable();	//DataTable to store temporary filtered girls in
        DataTable RGirlsCollection = new DataTable();		//DataTable to store random girls in
        DataTable ScriptsCollection = new DataTable();		//DataTable to store scripts in
        DataTable rgTable = new DataTable();				//table to store selected traits and chances for random girls
        SaveFileDialog Filesave = new SaveFileDialog();
        SaveFileDialog saveTraits = new SaveFileDialog();
        SaveFileDialog saveScript = new SaveFileDialog();
        string lastTip = "";								//temp storage string for specific item in listbox' to function
        string sConfigPath = "";
        string sTraitsPath = "";
        string sScriptPath = "";
        Random rnd = new Random();							//creates new random object to generate random values where needed
        bool LogAllCheckBoxes = false; bool LogOtherCheckBoxes = false;
        string[] sexskills = { "0", "0", "0", "0", "0", "0", "0", "0", "0", "0", "0" };

        public Form1()
        {
            InitializeComponent();
            comboBox_Girl_Type.SelectedIndex = 0;			//sets default selections for these comboBoxes
            comboBox_Girl_Accomm.SelectedIndex = 5;
            comboBox_ItemType_01.SelectedIndex = 3;
            comboBox_Rarity_05.SelectedIndex = 0;
            comboBox_ItemDuration.SelectedIndex = 0;
            comboBox_Horoscope_Type.SelectedIndex = 0;

            checkBox_ToggleTraitTooltips.Visible = false;	//hides traits checkbox by default
            StatusLabel1.Text = "";							//clears text in status bar

            TraitCollection.Columns.Add("Name", typeof(string));			//formatting of traits DataTable, adding two columns, Name and Data
            TraitCollection.Columns.Add("Desc", typeof(string));
            TraitCollection.Columns.Add("Type", typeof(string));
            TraitCollection.Columns.Add("InheritChance", typeof(string));
            TraitCollection.Columns.Add("RandomChance", typeof(string));
            TraitCollection.Columns.Add("Checked", typeof(string));

            dataGridView_Scripts.DataSource = ScriptsCollection;
            ScriptsCollection.Columns.Add("Script_ActionNumber", typeof(string));
            ScriptsCollection.Columns.Add("Script_EntryNumber", typeof(string));
            ScriptsCollection.Columns.Add("Script_Type", typeof(string));
            ScriptsCollection.Columns.Add("Script_IOValue", typeof(string));
            ScriptsCollection.Columns.Add("Script_Var", typeof(string));
            ScriptsCollection.Columns.Add("Script_Text", typeof(string));

            
            dataGridView_Traits.DataSource = TraitCollection;
            dataGridView_Traits.RowHeadersVisible = false;
            dataGridView_Traits.AllowUserToAddRows = true;
            dataGridView_Traits.Columns[0].Visible = true;
            dataGridView_Traits.Columns[1].Visible = true;
            dataGridView_Traits.Columns[2].Visible = true;
            dataGridView_Traits.Columns[3].Visible = true;
            dataGridView_Traits.Columns[4].Visible = true;
            dataGridView_Traits.Columns[5].Visible = false;

            TraitTypes.Columns.Add("CategoryName");
            dataGridView_Trait_Types.DataSource = TraitTypes;
            //dataGridView_Traits_Trait_Type.DataSource = TraitTypes;
            //dataGridView_Traits_Trait_Type.DisplayMember = "CategoryName";
            //dataGridView_Traits_Trait_Type.ValueMember = "CategoryName";
            comboBox_Traits_Trait_Type.DataSource = TraitTypes;
            comboBox_Traits_Trait_Type.DisplayMember = "CategoryName";
            comboBox_Traits_Trait_Type.ValueMember = "CategoryName";

            dataGridView_Trait_Types.RowHeadersVisible = false;
            dataGridView_Trait_Types.AllowUserToAddRows = true;
            dataGridView_Trait_Types.Columns[0].Visible = true;


            dataGridView_G_Traits.DataSource = TraitCollection;
            dataGridView_G_Traits.Columns[0].SortMode = DataGridViewColumnSortMode.Automatic;


            dataGridView_G_Traits.Columns[0].Visible = false;
            dataGridView_G_Traits.Columns[1].Visible = true;
            dataGridView_G_Traits.Columns[2].Visible = false;
            dataGridView_G_Traits.Columns[3].Visible = true;
            dataGridView_G_Traits.Columns[4].Visible = false;
            dataGridView_G_Traits.Columns[5].Visible = false;
            dataGridView_G_Traits.Columns[0].ReadOnly = true;
            dataGridView_G_Traits.Columns[1].ReadOnly = true;
            dataGridView_G_Traits.Columns[2].ReadOnly = true;
            dataGridView_G_Traits.Columns[3].ReadOnly = true;
            dataGridView_G_Traits.Columns[4].ReadOnly = true;
            dataGridView_G_Traits.Columns[5].ReadOnly = true;
            dataGridView_G_Traits.RowHeadersVisible = false;
            dataGridView_G_Traits.AllowUserToAddRows = false;
            dataGridView_G_Traits.AllowUserToDeleteRows = false;
            dataGridView_G_Traits.ShowCellToolTips = false;


            //Checks to determine editor executable is in one of predefined places so it can load CoreTraits.traits file automaticaly
            bool traitsloaded = false;
            //Try to load CoreTraits.traitsx file
            sTraitsPath = "Resources\\Data\\CoreTraits.traitsx";
            if (File.Exists(sTraitsPath)) { LoadTraitsXML(sTraitsPath); traitsloaded = true; }
            else if (File.Exists("..\\" + sTraitsPath)) { LoadTraitsXML("..\\" + sTraitsPath); traitsloaded = true; }
            else if (File.Exists("..\\..\\" + sTraitsPath)) { LoadTraitsXML("..\\..\\" + sTraitsPath); traitsloaded = true; }
            //Try the Default path to CoreTraits.traits file
            if (!traitsloaded)
            {
                sTraitsPath = "Resources\\Data\\CoreTraits.traits";
                if (File.Exists(sTraitsPath)) { LoadTraits(sTraitsPath); traitsloaded = true; }
                else if (File.Exists("..\\" + sTraitsPath)) { LoadTraits("..\\" + sTraitsPath); traitsloaded = true; }
                else if (File.Exists("..\\..\\" + sTraitsPath)) { LoadTraits("..\\..\\" + sTraitsPath); traitsloaded = true; }
            }
            //if check passes (file is found at this path) calls this function and sends this path to it, it's defined below            
            if (traitsloaded)
            {
                //if it finds it by itself it switches to Girls tab automatically, if neither of this combinations work it will default to Info tab
                tabControl1.SelectedTab = tabPage1_Girls;
                checkBox_ToggleTraitTooltips.Visible = true;
            }

            

            //checks where config.xml is
            bool configfound = false;
            //Default path to config.xml
            string sConfigPath = "config.xml";
            if (File.Exists(sConfigPath)) { LoadConfigXML(sConfigPath); configfound = true; }
            else if (File.Exists("..\\" + sConfigPath)) { LoadConfigXML("..\\" + sConfigPath); configfound = true; }
            else if (File.Exists("..\\..\\" + sConfigPath)) { LoadConfigXML("..\\..\\" + sConfigPath); configfound = true; }
            if (!configfound)   //Old Default path to config.xml
            {
                sConfigPath = "Resources\\Data\\config.xml";
                if (File.Exists(sConfigPath)) { LoadConfigXML(sConfigPath); configfound = true; }
                else if (File.Exists("..\\" + sConfigPath)) { LoadConfigXML("..\\" + sConfigPath); configfound = true; }
                else if (File.Exists("..\\..\\" + sConfigPath)) { LoadConfigXML("..\\..\\" + sConfigPath); configfound = true; }
            }
            //string sInterfacePath = "Resources\\Interface";

            rgTable.Columns.Add("Trait", typeof(string));			//adds columns to rgTable dataTable
            rgTable.Columns.Add("Chance", typeof(int));
            // links datagrid on form with this dataTable, below are some options to set behaviour of this datagrid
            dataGridView1.DataSource = rgTable;
            dataGridView1.Columns[0].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            dataGridView1.Columns[0].ReadOnly = true;
            dataGridView1.Columns[1].AutoSizeMode = DataGridViewAutoSizeColumnMode.AllCells;
            dataGridView1.Columns[1].ReadOnly = true;
            dataGridView1.RowHeadersVisible = false;
            dataGridView1.AllowUserToAddRows = false;


            iTable.Columns.Add("Type", typeof(string));					//Adding columns to items effects table
            iTable.Columns.Add("Attribute", typeof(string));
            iTable.Columns.Add("Value", typeof(string));
            dataGridView2.DataSource = iTable;
            dataGridView2.RowHeadersVisible = false;
            dataGridView2.AllowUserToAddRows = false;
            dataGridView2.Columns[0].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            dataGridView2.Columns[0].ReadOnly = true;
            dataGridView2.Columns[1].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            dataGridView2.Columns[1].ReadOnly = true;
            dataGridView2.Columns[2].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            dataGridView2.Columns[2].ReadOnly = true;

            iTTable.Columns.Add("Traits", typeof(string));				//Adding columns to items traits table
            iTTable.Columns.Add("Value", typeof(string));
            iTTable.Columns.Add("Duration", typeof(string));
            dataGridView3.DataSource = iTTable;
            dataGridView3.RowHeadersVisible = false;
            dataGridView3.AllowUserToAddRows = false;
            dataGridView3.Columns[0].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            dataGridView3.Columns[0].ReadOnly = true;
            dataGridView3.Columns[1].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            dataGridView3.Columns[1].ReadOnly = true;
            dataGridView3.Columns[2].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
            dataGridView3.Columns[2].ReadOnly = true;
            dataGridView3.Columns[2].Visible = false;

            ItemsCollection.Columns.Add("Name", typeof(string));			//formatting of items DataTable, adding two columns, Name and Data
            ItemsCollection.Columns.Add("Data", typeof(string));

            GirlsCollection.Columns.Add("Name", typeof(string));			//formatting of girls DataTable, adding two columns, Name and Data
            GirlsCollection.Columns.Add("Data", typeof(string));
            GirlsCollection.Columns.Add("TypeFilter", typeof(string));	//this was added at later point, in this column goes girl type (slave, normal...) so they could be easily filtered sorted by that key

            RGirlsCollection.Columns.Add("Name", typeof(string));		//formatting of random girls DataTable, adding two columns, Name and Data
            RGirlsCollection.Columns.Add("Data", typeof(string));

            this.comboBox_SortByType.SelectedIndexChanged -= new System.EventHandler(this.comboBox_SortByType_SelectedIndexChanged);
            comboBox_SortByType.SelectedIndex = 0;				//sets girl type droplist to show "All" as default
            this.comboBox_SortByType.SelectedIndexChanged += new System.EventHandler(this.comboBox_SortByType_SelectedIndexChanged);
        }
        /* `J` Random number generator 
         * between min and max, skewed by, and type:
         * 0: Flat skew  - basically add or subtract after the random
         * 1: Round down to skew - round down to nearest skew
         * 2: Round up to skew - round up to nearest skew
         * 3: Round to nearest skew - round to nearest skew
         * 4: Random one way skew
         * 5: Random two way skew
         * 6: Scale toward skew
         * 
         * bonus is applied after all skews
         * round is applied after all unless skew type is round
         * * valid round strings are "" "U5" "D5" "U10" "D10" "N5" "N10"
        */
        private string J_Rand(int min = 0, int max = 100, int skew = 0, int type = 0, int bonus = 0, string round = "")
        {
            int negadj = 0;
            if (min < 0)
            {
                negadj -= min;
                min += negadj;
                max += negadj;
            }
            int ss = rnd.Next(min, max + 1);
            if (type == 0) ss += skew;
            else if (type == 4 && skew < 0) ss += rnd.Next(skew, 0);
            else if (type == 4 && skew > 0) ss += rnd.Next(0, skew);
            else if (type == 5 && skew < 0) ss += rnd.Next(skew, -skew);
            else if (type == 5 && skew > 0) ss += rnd.Next(-skew, skew);
            else if (type == 6)
            {
                int i = 0;
                int j = skew - ss;
                if (j > 0) i = rnd.Next(0, j);
                else if (j < 0) i = rnd.Next(j, 0);
                ss += i;
            }
            // round if called for
            if (type == 1 || type == 2 || type == 3 || round != "")// type: 1=down, 2=up, 3=nearest
            {
                int rt = 3; int rn = 1;
                if (type == 1 || type == 2 || type == 3)
                {
                    rt = type;
                    rn = skew;
                }
                else if (round.Length > 0)
                {
                    switch (round.Substring(0, 1))
                    {
                        case "d":
                        case "D":
                            rt = 1;
                            break;
                        case "u":
                        case "U":
                            rt = 2;
                            break;
                        default:
                            rt = 3;
                            break;
                    }
                }
                if (round.Length > 0)
                {
                    if (round.Substring(1, round.Length - 1) == "10") rn = 10;
                    else if (round.Substring(1, round.Length - 1) == "5") rn = 5;
                    else
                    {
                        string rnt = "";
                        for (int i = 0; i < round.Length; i++)
                        {
                            string a = round.Substring(i, 1);
                            switch (a)
                            {
                                case "0":
                                case "1":
                                case "2":
                                case "3":
                                case "4":
                                case "5":
                                case "6":
                                case "7":
                                case "8":
                                case "9":
                                    if (rnt.Length == 0 && a == "0") break; // skip leading zeros
                                    rnt = rnt + "" + a;
                                    break;
                            }
                        }
                        if (rnt.Length > 0) rn = Convert.ToInt32(rnt);
                    }
                }
                if (rn < 0) rn = -rn;
                if (rn > 1)
                {
                    int up = 0;
                    float i = (float)ss / (float)rn;
                    if (rt == 2 || (rt == 3 && (int)((i - (int)i) * 10) > 5)) up = 1;
                    ss = ((int)i + up) * (rn);
                }
            }
            ss += bonus;    // this could throw off the rounding
            if (ss < min) ss = min;
            if (ss > max) ss = max;
            if (ss < 0) ss = 0;     // nothing should be negative
            ss -= negadj;           // unless min was negative
            return ss.ToString();
        }

        //************************************
        //*****   Load and Save TRAITS   *****
        //************************************

        //Load traits button, if it couldn't find it by itself user will have to find it manually, basically it calls LoadTraits function with path to the file user browsed to
        private void button_Load_Traits_Click(object sender, EventArgs e)
        {
            OpenFileDialog openTraits = new OpenFileDialog();
            openTraits.Filter = "Traits xml|*.traitsx|Traits Plain Text|*.traits|All Files|*.*";
            openTraits.ShowDialog();
            try
            {
                switch (Path.GetExtension(openTraits.FileName))
                {
                    case ".traits":
                        LoadTraits(openTraits.FileName);	//calls LoadGirls function and passes file name with path to it
                        break;
                    case ".traitsx":
                        LoadTraitsXML(openTraits.FileName);
                        break;
                }
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(openTraits.FileName, err.Message);
                MessageBox.Show("You have to Select CoreTraits.traits file to use for traits database!\n\rOr if you did and you still got this message there could be a problem with file permissions.\n\rNext is the \"official\" error:\n\r\n\r" + err.Message, "Traits file error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }
        private void button_Save_Traits_Click(object sender, EventArgs e)
        {
            saveTraits.Filter = "Whore Master XML traits file|*.traitsx|Whore Master traits file|*.traits|All files|*.*";
            saveTraits.DefaultExt = ".traitsx";
            if (sTraitsPath != "")
            {
                saveTraits.FileName = Path.GetFileName(sTraitsPath);
                saveTraits.InitialDirectory = Path.GetDirectoryName(sTraitsPath);
            }
            try
            {
                saveTraits.ShowDialog();
                if (File.Exists(Convert.ToString(saveTraits.FileName) + ".bak") == true) File.Delete(Convert.ToString(saveTraits.FileName) + ".bak");
                if (File.Exists(Convert.ToString(saveTraits.FileName)) == true) File.Move(Convert.ToString(saveTraits.FileName), Convert.ToString(saveTraits.FileName) + ".bak");
                switch (Path.GetExtension(saveTraits.FileName))
                {
                    case ".traits":
                        SaveTraits(saveTraits.FileName);
                        break;
                    case ".traitsx":
                    default:
                        SaveTraitsXML(saveTraits.FileName);
                        break;
                }
                StatusLabel1.Text = "Traits XML saved...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(saveTraits.FileName, err.Message);
                StatusLabel1.Text = "XML Trait save error...";
            }
        }
        private void LoadTraits(string path)
        {
            try
            {
                StreamReader Import = new StreamReader(path);			//opens StreamReader and points it to path recieved when function was called
                while (Import.Peek() >= 0)								//basically do while EoF
                {
                    string temp1 = Convert.ToString(Import.ReadLine());	//reads first line from traits file (this is trait name), it could be added directly to trait trait list, but this way I can use it multiple times (to fill traits tab and trait tooltips)
                    string temp2 = Convert.ToString(Import.ReadLine());	//second line this is trait description
                    //                    ListBox_G_Traits.Items.Add(temp1);					//adds trait to checkedListBox for girls
                    TraitCollection.Rows.Add(temp1, temp2);
                    comboBox_ItemTraits.Items.Add(temp1);				//fills droplist on item tab with traits
                    comboBox_RGTraits.Items.Add(temp1);					//same with droplist on random girls tab
                    aTraits.Add(temp2);									//fills aTraits ArrayList with traits description, they are called as traits tooltips description, each trait in listbox has the same index as description here so it's easy to link them
                }
                Import.Close();
                StatusLabel1.Text = "Loaded traits file...";				//updates status line with this message
                comboBox_RGTraits.SelectedIndex = 0;						//after filling random girls droplist it sets it to first entry
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("File load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "Open trait file error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        private void LoadTraitsXML(string path)     // `J` added
        {
            try
            {
                XmlTextReader xmlread = new XmlTextReader(path);		//Creates XmlTextReader instance to open/read XML file
                XmlDocument xmldoc = new XmlDocument();					//Creates XmlDocument instance that will be used to go through XML file
                xmldoc.Load(xmlread);
                XmlNode baseNode = xmldoc.DocumentElement;				//This one isolates one node in our XmlDocument
                int x = 0;												//this is just a counter, it gets incremented for each "trait" read, so at the end we have a number of traits loaded, this is only used to display in status bar how many traits were loaded
                foreach (XmlNode node in baseNode.SelectNodes("/Traits/Trait"))
                {
                    x++;
                    string sName = "", sDesc = "", sType = "", sInheritChance = "", sRandomChance = "";
                    for (int i = 0; i < node.Attributes.Count; i++)
                    {
                        if (node.Attributes[i].Name == "Name") sName = node.Attributes["Name"].Value;
                        if (node.Attributes[i].Name == "Desc") sDesc = node.Attributes["Desc"].Value;
                        if (node.Attributes[i].Name == "Type")
                        {
                            sType = node.Attributes["Type"].Value;
                            AddTraitType(sType);
                        }
                        if (node.Attributes[i].Name == "InheritChance") sInheritChance = node.Attributes["InheritChance"].Value;
                        if (node.Attributes[i].Name == "RandomChance") sRandomChance = node.Attributes["RandomChance"].Value;
                    }
                    //                    ListBox_G_Traits.Items.Add(sName);
                    TraitCollection.Rows.Add(sName, sDesc, sType, sInheritChance, sRandomChance);
                    comboBox_ItemTraits.Items.Add(sName);
                    comboBox_RGTraits.Items.Add(sName);
                    aTraits.Add(sDesc);
                }
                xmlread.Close();
                //dataGridView_Traits_Trait_Type.DataSource = TraitTypes;
                comboBox_Traits_Trait_Type.DataSource = TraitTypes;
                StatusLabel1.Text = "Loaded " + x.ToString() + " traits from XML file...";
                comboBox_RGTraits.SelectedIndex = 0;	//after filling random girls droplist it sets it to first entry
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("XML load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "XML Trait load error error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "XML Trait load error...";
            }
        }
        private void SaveTraits(string path)
        {
            string output = "";                                         //this is the string to which output from following for loop will be saved, more precisely what will be save in final file
            for (int i = 0; i < TraitCollection.Rows.Count; i++)     //again, no philosophy in this one, get the number of girls in the list, and for every one of them do the following, which is take name from dataTable and add it to output, "press" enter, add data from dataTable, this time there's no need for "enter", girl data string has one at the end, it turned out to be best this way, no need to worry if new girl added to the output will be in new line, this way it's automatic, although this is not necessarily the best way
            {
                if (TraitCollection.Rows.Count == i + 1) output = output + TraitCollection.Rows[i][0] + "\r\n" + TraitCollection.Rows[i][1];
                else output = output + TraitCollection.Rows[i][0] + "\r\n" + TraitCollection.Rows[i][1] + "\r\n";
            }
            output = output.TrimEnd('\r', '\n');                        //when whole output string is done there will be an extra "enter" ("\r\n") at the end from the last girl dataTable data, this will mess up the game if there's an extra empty line at the end, but since I KNOW that it's there it's easily removable, this line does that. This could have been done differently, like remove that last "\r\n" from girl data, and then add it through previous loop with check that in case it's last girl don't add "\r\n" at the end, but this works, and it's one if less
            StreamWriter Export = new StreamWriter(Convert.ToString(saveTraits.FileName));
            Export.Write(output);
            Export.Close();
        }
        private void button_Clear_Traits_Click(object sender, EventArgs e)
        {
            // zzzzzzzzzzzz needs work
            TraitCollection.Clear();
        }
        private void SaveTraitsXML(string path)
        {
            XmlDocument xmldoc = new XmlDocument();
            XmlElement traits = xmldoc.CreateElement("Traits");
            XmlElement trait = xmldoc.CreateElement("Trait");
            xmldoc.AppendChild(traits);
            string sName = "", sDesc = "", sType = "", sInheritChance = "", sRandomChance = "";
                    
            for (int x = 0; x < TraitCollection.Rows.Count; x++)
            {
                if (TraitCollection.Rows[x]["Name"].ToString() == "")
                    continue;
                trait = xmldoc.CreateElement("Trait");
                sName = TraitCollection.Rows[x]["Name"].ToString();
                sDesc = TraitCollection.Rows[x]["Desc"].ToString();
                sType = TraitCollection.Rows[x]["Type"].ToString();
                sInheritChance = TraitCollection.Rows[x]["InheritChance"].ToString();
                sRandomChance = TraitCollection.Rows[x]["RandomChance"].ToString();
                trait.SetAttribute("Name", sName);
                trait.SetAttribute("Desc", sDesc);
                trait.SetAttribute("Type", sType);
                trait.SetAttribute("InheritChance", sInheritChance);
                trait.SetAttribute("RandomChance", sRandomChance);
                traits.AppendChild(trait);
            }
            //after it's all done we have our XML, although, only in memory, not stored somewhere safe
            XmlWriterSettings settings = new XmlWriterSettings();	//I've tried few ways of saving this, and this had the nicest output (they were all correct XML wise mind you, but output of this has the "nicest" structure as far as human readability goes
            settings.Indent = true;									//indent every node, otherwise it would be harder to find where each node begins, again, not for computer, for some person looking at outputed XML
            settings.NewLineOnAttributes = true;					//without this each node would be one long line, this puts each attribute in new line
            settings.IndentChars = "    ";							//just a character that'll be used for indenting, \t means tab, so indent is one tab,
            XmlWriter xmlwrite = XmlWriter.Create(path, settings);	//now that settings are complete we can write this file, using path passed from button function, and settings we just made
            xmldoc.Save(xmlwrite);									//now we tell our XmlDocument to save itself to our XmlWriter, this is what finally gives us our file
            xmlwrite.Close();										//now to be all nice and proper we close our file, after all it's finished
        }

        private void AddTraitType(string type)
        {
            for (int x = 0; x < TraitTypes.Rows.Count;x++ )
            {
                if (TraitTypes.Rows[x]["CategoryName"].ToString() == type)
                {
                    return;
                }
            }
            DataRow dr = TraitTypes.NewRow();
            dr["CategoryName"] = type;
            TraitTypes.Rows.InsertAt(dr, 0);
        }

        private void dataGridView_Trait_Types_SelectedIndexChanged(object sender, EventArgs e)
        {
            DataTable dt = TraitCollection;
            SortDataTable(ref dt, ref listBox_Traits_Names);
            dataGridView_Trait_Types.DataSource = TraitTypes;
            //dataGridView_Traits_Trait_Type.DataSource = TraitTypes;

            //dataGridView_Traits.DataSource = TraitCollection;
        }

        private void listBox_Trait_Names_SelectedIndexChanged(object sender, EventArgs e)
        {
            int num = listBox_Traits_Names.SelectedIndex;
            if (num < 0) return;
            textBox_Traits_Trait_Name.Text = TraitCollection.Rows[num]["Name"].ToString();
            TBox_Traits_Description.Text = TraitCollection.Rows[num]["Desc"].ToString();
            comboBox_Traits_Trait_Type.SelectedIndex = comboBox_Traits_Trait_Type.FindStringExact(TraitCollection.Rows[num]["Type"].ToString());
            textBox_Traits_Inherit_Chance.Text = TraitCollection.Rows[num]["InheritChance"].ToString();
            textBox_Traits_Random_Chance.Text = TraitCollection.Rows[num]["RandomChance"].ToString();




        }



        //*******************************************
        //*****   Load, Save and Reset CONFIG   *****
        //*******************************************

        private void button_Load_Config_Click(object sender, EventArgs e)
        {
            OpenFileDialog openConfig = new OpenFileDialog();
            openConfig.Filter = "Whore Master XML config file|*.xml|All files|*.*";
            openConfig.InitialDirectory = Application.StartupPath;
            try
            {
                openConfig.ShowDialog();
                LoadConfigXML(openConfig.FileName);
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(openConfig.FileName, err.Message);
                StatusLabel1.Text = "Load Error.";
            }
        }
        private void button_Config_Save_Click(object sender, EventArgs e)
        {
            SaveFileDialog saveConfig = new SaveFileDialog();
            saveConfig.Filter = "Whore Master XML config file|*.xml|All files|*.*";
            saveConfig.DefaultExt = ".xml";
            if (sConfigPath != "")
            {
                saveConfig.FileName = Path.GetFileName(sConfigPath);
                saveConfig.InitialDirectory = Path.GetDirectoryName(sConfigPath);
            }
            try
            {
                saveConfig.ShowDialog();
                if (File.Exists(Convert.ToString(saveConfig.FileName) + ".bak") == true) File.Delete(Convert.ToString(saveConfig.FileName) + ".bak");
                if (File.Exists(Convert.ToString(saveConfig.FileName)) == true) File.Move(Convert.ToString(saveConfig.FileName), Convert.ToString(saveConfig.FileName) + ".bak");
                SaveConfigXML(saveConfig.FileName);
                StatusLabel1.Text = "Config.xml saved...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(saveConfig.FileName, err.Message);
                StatusLabel1.Text = "Save Config Error...";
            }
        }
        private void button_Config_Reset_Click(object sender, EventArgs e)
        {
            ConfigReset();
        }

        //loads config.xml
        private void LoadConfigXML(string path)
        {
            try
            {
                sConfigPath = Path.GetFullPath(path);

                XmlTextReader xmlread = new XmlTextReader(path);
                XmlDocument xmldoc = new XmlDocument();
                xmldoc.Load(xmlread);
                XmlNode baseNode = xmldoc.DocumentElement;
                XmlNode n;  // node
                string nv;  // node value
                int nvi;    // node value int
                double nvd; // node value double
                bool nvtf;  // node value true/false
                n = baseNode.SelectSingleNode("/config/Folders");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value; if (nv == "true") nvtf = true; else nvtf = false;
                        switch (n.Attributes[i].Name)
                        {
                            case "Characters": textBox_Config_Folders_Characters.Text = nv; break;
                            case "Saves": textBox_Config_Folders_Savegames.Text = nv; break;
                            case "DefaultImages": textBox_Config_Folders_Default_Images.Text = nv; break;
                            case "Items": textBox_Config_Folders_Items.Text = nv; break;
                            case "BackupSaves": checkBox_Config_Folders_Backup_Saves.Checked = nvtf; break;
                            case "PreferDefault": checkBox_Config_Folders_Prefer_Default.Checked = nvtf; break;
                        }
                    }
                }
                
                n = baseNode.SelectSingleNode("/config/Resolution");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value; if (nv == "true") nvtf = true; else nvtf = false;
                        switch (n.Attributes[i].Name)
                        {
                            case "Resolution": textBox_Config_Resolution.Text = nv; break;
                            case "Width": textBox_Config_Screen_Width.Text = nv; break;
                            case "Height": textBox_Config_Screen_Height.Text = nv; break;
                            case "ScaleWidth": textBox_Config_Screen_Width_Scale.Text = nv; break;
                            case "ScaleHeight": textBox_Config_Screen_Height_Scale.Text = nv; break;
                            case "FullScreen": checkBox_Config_FullScreen.Checked = nvtf; break;
                            case "ListScrollAmount": textBox_Config_List_Scroll_Amount.Text = nv; break;
                            case "TextScrollAmount": textBox_Config_Text_Scroll_Amount.Text = nv; break;
                        }
                    }
                }
                if (textBox_Config_Screen_Width_Scale.Text == "") textBox_Config_Screen_Width_Scale.Text = "800";
                if (textBox_Config_Screen_Height_Scale.Text == "") textBox_Config_Screen_Height_Scale.Text = "600";
                
                n = baseNode.SelectSingleNode("/config/Initial");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%'); if (nv == "true") nvtf = true; else nvtf = false;
                        switch (n.Attributes[i].Name)
                        {
                            case "Gold":                    textBox_Initial_Gold.Text = nv; break;
                            case "GirlMeet":                textBox_Config_InitialGirlMeet.Text = nv; break;
                            case "GirlsHousePerc":          textBox_Config_InitialGirlsHousePerc.Text = nv; break;
                            case "GirlsKeepTips":           checkBox_Config_GirlsKeepTips.Checked = nvtf; break;
                            case "SlavePayOutOfPocket":     checkBox_Config_SlavesPayOutOfPocket.Checked = nvtf; break;
                            case "SlaveHousePerc":          textBox_Config_InitialSlaveHousePerc.Text = nv; break;
                            case "SlaveKeepTips":           checkBox_Config_SlavesKeepTips.Checked = nvtf; break;
                            case "GirlsAccom":              comboBox_Config_InitialGirlsAccom.SelectedIndex = Convert.ToInt32(nv); break;
                            case "SlaveAccom":              comboBox_Config_InitialSlaveAccom.SelectedIndex = Convert.ToInt32(nv); break;
                            case "AutoUseItems":            checkBox_Config_AutoUseItems.Checked = nvtf; break;
                            case "AutoCombatEquip":         checkBox_Config_AutoEquipCombat.Checked = nvtf; break;
                            case "TortureTraitWeekMod":     textBox_Torture_Mod.Text = nv; break;
                            case "HoroscopeType":           comboBox_Horoscope_Type.SelectedIndex = nv == "2" ? 1 : 0; break;

                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Income");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%'); if (nv == "true") nvtf = true; else nvtf = false;
                        switch (n.Attributes[i].Name)
                        {
                            case "ExtortionIncome": textBox_Config_IncomeExtortion.Text = nv; break;
                            case "GirlsWorkBrothel": textBox_Config_IncomeBrothel.Text = nv; break;
                            case "GirlsWorkStreet": textBox_Config_IncomeStreet.Text = nv; break;
                            case "MovieIncome": textBox_Config_IncomeMovie.Text = nv; break;
                            case "StripperIncome": textBox_Config_IncomeStripper.Text = nv; break;
                            case "BarmaidIncome": textBox_Config_IncomeBarmaid.Text = nv; break;
                            case "SlaveSales": textBox_Config_IncomeSlaveSales.Text = nv; break;
                            case "ItemSales": textBox_Config_IncomeItemSales.Text = nv; break;
                            case "ClinicIncome": textBox_Config_IncomeClinic.Text = nv; break;
                            case "ArenaIncome": textBox_Config_IncomeArena.Text = nv; break;
                            case "FarmIncome": textBox_Config_IncomeFarm.Text = nv; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Expenses");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%');
                        switch (n.Attributes[i].Name)
                        {
                            case "Training": textBox_Config_ExpenseTraining.Text = nv; break;
                            case "MovieCost": textBox_Config_ExpenseMovie.Text = nv; break;
                            case "ActressWages": textBox_Config_ExpenseActress.Text = nv; break;
                            case "GoonWages": textBox_Config_ExpenseGoon.Text = nv; break;
                            case "MatronWages": textBox_Config_ExpenseMatron.Text = nv; break;
                            case "StaffWages": textBox_Config_ExpenseStaff.Text = nv; break;
                            case "GirlSupport": textBox_Config_ExpenseGirls.Text = nv; break;
                            case "Consumables": textBox_Config_ExpenseConsumables.Text = nv; break;
                            case "Items": textBox_Config_ExpenseItems.Text = nv; break;
                            case "SlavesBought": textBox_Config_ExpenseSlaves.Text = nv; break;
                            case "BuyBrothel": textBox_Config_ExpenseBrothelPrice.Text = nv; break;
                            case "BrothelSupport": textBox_Config_ExpenseBrothel.Text = nv; break;
                            case "BarSupport": textBox_Config_ExpenseBar.Text = nv; break;
                            case "CasinoSupport": textBox_Config_ExpenseCasino.Text = nv; break;
                            case "Bribes": textBox_Config_ExpenseBribes.Text = nv; break;
                            case "Fines": textBox_Config_ExpenseFines.Text = nv; break;
                            case "Advertising": textBox_Config_ExpenseAdvertising.Text = nv; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Gambling");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%');
                        switch (n.Attributes[i].Name)
                        {
                            case "Odds": textBox_Config_GamblingOdds.Text = nv; break;
                            case "Base": textBox_Config_GamblingBase.Text = nv; break;
                            case "Spread": textBox_Config_GamblingSpread.Text = nv; break;
                            case "CustomerFactor": textBox_Config_GamblingCustomer.Text = nv; break;
                            case "HouseFactor": textBox_Config_GamblingHouse.Text = nv; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Prostitution");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%');
                        switch (n.Attributes[i].Name)
                        {
                            case "RapeBrothel": textBox_Config_ProstitutionRapeBrothel.Text = nv; break;
                            case "RapeStreet": textBox_Config_ProstitutionRapeStreets.Text = nv; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Catacombs");
                if (n != null)
                {
                    double girlgirls = 1, girlitems = 1, girlbeast = 1, ganggirls = 1, gangitems = 1, gangbeast = 1;
                    // load data
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%'); if (nv == "true") nvtf = true; else nvtf = false;
                        int.TryParse(nv, out nvi);
                        double.TryParse(nv, out nvd);
                        switch (n.Attributes[i].Name)
                        {
                            case "UniqueCatacombs": textBox_Config_Unique_Catacombs.Text = nv; break;
                            case "ControlGirls": checkBox_Config_Control_Girls.Checked = nvtf; break;
                            case "ControlGangs": checkBox_Config_Control_Gangs.Checked = nvtf; break;
                            case "GirlGetsGirls": girlgirls = nvd; break;
                            case "GirlGetsItems": girlitems = nvd; break;
                            case "GirlGetsBeast": girlbeast = nvd; break;
                            case "GangGetsGirls": ganggirls = nvd; break;
                            case "GangGetsItems": gangitems = nvd; break;
                            case "GangGetsBeast": gangbeast = nvd; break;
                        }
                    }
                    // validate data
                    if (girlgirls < 0) girlgirls = -girlgirls; if (girlitems < 0) girlitems = -girlitems; if (girlbeast < 0) girlbeast = -girlbeast;
                    if (ganggirls < 0) ganggirls = -ganggirls; if (gangitems < 0) gangitems = -gangitems; if (gangbeast < 0) gangbeast = -gangbeast;

                    double checkggirl = girlgirls + girlitems + girlbeast;
                    if (checkggirl == 0) girlgirls = girlitems = girlbeast = (100 / 3);
                    else
                    {
                        girlgirls = ((int)((100.0 / checkggirl) * girlgirls)) + (girlgirls == (int)girlgirls ? 0 : 1);
                        girlitems = (int)((100.0 / checkggirl) * girlitems);
                        girlbeast = 100 - (int)girlgirls - (int)girlitems;
                    }
                    double checkggang = ganggirls + gangitems + gangbeast;
                    if (checkggang == 0) ganggirls = gangitems = gangbeast = (100 / 3);
                    else
                    {
                        ganggirls = ((int)((100.0 / checkggang) * ganggirls)) + (ganggirls == (int)ganggirls ? 0 : 1);
                        gangitems = (int)((100.0 / checkggang) * gangitems);
                        gangbeast = 100 - (int)ganggirls - (int)gangitems;
                    }									 
                    // set trackbars
                    trackBar_Config_Control_GirlsA.Value = (int)girlgirls;
                    trackBar_Config_Control_GirlsB.Value = (int)girlgirls + (int)girlitems;
                    trackBar_Config_Control_GangsA.Value = (int)ganggirls;
                    trackBar_Config_Control_GangsB.Value = (int)ganggirls + (int)gangitems;
                    // set texts
                    Label_Config_Control_Girls_Girls.Text = "Girl: "  + (int)girlgirls + "%";
                    Label_Config_Control_Girls_Items.Text = "Item: "  + (int)girlitems + "%";
                    Label_Config_Control_Girls_Beast.Text = "Beast: " + (int)girlbeast + "%";
                    Label_Config_Control_Gangs_Girls.Text = "Girl: "  + (int)ganggirls + "%";
                    Label_Config_Control_Gangs_Items.Text = "Item: "  + (int)gangitems + "%";
                    Label_Config_Control_Gangs_Beast.Text = "Beast: " + (int)gangbeast + "%";
                }

                n = baseNode.SelectSingleNode("/config/SlaveMarket");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%');
                        int.TryParse(nv, out nvi);
                        switch (n.Attributes[i].Name)
                        {
                            case "UniqueMarket": textBox_Config_Unique_Market.Text = nv; break;
                            case "SlavesNewWeeklyMin": trackBar_Config_Market_Min.Value = nvi; break;
                            case "SlavesNewWeeklyMax": trackBar_Config_Market_Max.Value = nvi; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Pregnancy");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%');
                        switch (n.Attributes[i].Name)
                        {
                            case "PlayerChance": textBox_Config_PregnancyPlayer.Text = nv; break;
                            case "CustomerChance": textBox_Config_PregnancyCustomer.Text = nv; break;
                            case "MonsterChance": textBox_Config_PregnancyMonster.Text = nv; break;
                            case "GoodSexFactor": textBox_Config_PregnancyGoodSex.Text = nv; break;
                            case "ChanceOfGirl": textBox_Config_PregnancyGirlChance.Text = nv; break;
                            case "WeeksPregnant": textBox_Config_PregnancyWeeksPregnant.Text = nv; break;
                            case "WeeksMonsterP": textBox_Config_PregnancyWeeksMonsterP.Text = nv; break;
                            case "MiscarriageChance": textBox_Config_PregnancyMiscarriageChance.Text = nv; break;
                            case "MiscarriageMonster": textBox_Config_PregnancyMiscarriageMonster.Text = nv; break;
                            case "WeeksTillGrown": textBox_Config_PregnancyWeeksTillGrown.Text = nv; break;
                            case "CoolDown": textBox_Config_PregnancyCoolDown.Text = nv; break;
                            case "AntiPregFailure": textBox_Config_AntiPregFailure.Text = nv; break;
                            case "MultiBirthChance": textBox_Config_Multi_Birth_Chance.Text = nv; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Tax");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%');
                        switch (n.Attributes[i].Name)
                        {
                            case "Rate": textBox_Config_TaxRate.Text = nv; break;
                            case "Minimum": textBox_Config_TaxMinimum.Text = nv; break;
                            case "Laundry": textBox_Config_TaxLaundry.Text = nv; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Gangs");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value.TrimEnd('%');
                        switch (n.Attributes[i].Name)
                        {
                            case "MaxRecruitList": textBox_config_MaxRecruitList.Text = nv; break;
                            case "StartRandom": textBox_config_StartRandom.Text = nv; break;
                            case "StartBoosted": textBox_config_StartBoosted.Text = nv; break;
                            case "InitMemberMin": textBox_config_InitMemberMin.Text = nv; break;
                            case "InitMemberMax": textBox_config_InitMemberMax.Text = nv; break;
                            case "ChanceRemoveUnwanted": textBox_config_ChanceRemoveUnwanted.Text = nv; break;
                            case "AddNewWeeklyMin": textBox_config_AddNewWeeklyMin.Text = nv; break;
                            case "AddNewWeeklyMax": textBox_config_AddNewWeeklyMax.Text = nv; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Fonts");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value; if (nv == "true") nvtf = true; else nvtf = false;
                        switch (n.Attributes[i].Name)
                        {
                            case "Normal": textBox_Config_FontNormal.Text = nv; break;
                            case "Fixed": textBox_config_FontFixed.Text = nv; break;
                            case "Antialias": checkBox_Config_Antialias.Checked = nvtf; break;
                            case "ShowPercent": checkBox_Config_Show_Percent.Checked = nvtf; break;
                        }
                    }
                }

                n = baseNode.SelectSingleNode("/config/Debug");
                if (n != null)
                {
                    for (int i = 0; i < n.Attributes.Count; i++)
                    {
                        nv = n.Attributes[i].Value; if (nv == "true") nvtf = true; else nvtf = false;
                        switch (n.Attributes[i].Name)
                        {
                            case "LogAll": checkBox_config_LogAll.Checked = nvtf; break;
                            case "LogGirls": checkBox_config_LogGirls.Checked = nvtf; break;
                            case "LogRGirls": checkBox_config_LogRGirls.Checked = nvtf; break;
                            case "LogGirlFights": checkBox_config_LogGirlFights.Checked = nvtf; break;
                            case "LogItems": checkBox_config_LogItems.Checked = nvtf; break;
                            case "LogFonts": checkBox_config_LogFonts.Checked = nvtf; break;
                            case "LogTorture": checkBox_config_LogTorture.Checked = nvtf; break;
                            case "LogDebug": checkBox_config_LogDebug.Checked = nvtf; break;
                            case "LogExtraDetails": checkBox_config_LogExtraDetails.Checked = nvtf; break;
                            case "LogShowNumbers": checkBox_config_LogShowNumbers.Checked = nvtf; break;
                        }
                    }
                }

                switch (StatusLabel1.Text)
                {
                    case "Loaded traits file...":
                        StatusLabel1.Text = "Loaded traits file and config file...";
                        break;
                    default:
                        StatusLabel1.Text = "Loaded config file...";
                        break;
                }
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(sConfigPath, err.Message);
                StatusLabel1.Text = "LoadConfigXML Error...";
            }
        }
        //saves config.xml
        private void SaveConfigXML(string path)
        {
            XmlDocument xmldoc = new XmlDocument();

            XmlElement xeConfig = xmldoc.CreateElement("config");
            XmlElement xeFolders = xmldoc.CreateElement("Folders");
            XmlElement xeResolution = xmldoc.CreateElement("Resolution");
            XmlElement xeInitial = xmldoc.CreateElement("Initial");
            XmlElement xeIncome = xmldoc.CreateElement("Income");
            XmlElement xeExpenses = xmldoc.CreateElement("Expenses");
            XmlElement xeGambling = xmldoc.CreateElement("Gambling");
            XmlElement xeTax = xmldoc.CreateElement("Tax");
            XmlElement xePregnancy = xmldoc.CreateElement("Pregnancy");
            XmlElement xeProstitution = xmldoc.CreateElement("Prostitution");
            XmlElement xeCatacombs = xmldoc.CreateElement("Catacombs");
            XmlElement xeSlaveMarket = xmldoc.CreateElement("SlaveMarket");
            XmlElement xeGangs = xmldoc.CreateElement("Gangs");
            XmlElement xeItems = xmldoc.CreateElement("Items");
            XmlElement xeFonts = xmldoc.CreateElement("Fonts");
            XmlElement xeDebug = xmldoc.CreateElement("Debug");

            XmlComment xcFolders = xmldoc.CreateComment("\n\tCharacters     = The location of the Characters folder\n\tSaves          = The location of the Saves folder\n\tDefaultImages  = The location of the DefaultImages folder\n\tItems          = The location of the Items folder\n\t\t\tThe folders can be relative to the EXE file or an exact folder path.\n\t\t\t\tRelative folders refers to its location to the parent folder of the game\n\t\t\t\t\tUse  '..\\Characters'  and the like.\n\t\t\t\tAbsolute folders is the full path name as defined by the operating system.\n\t\t\t\t\tUse  'c:\\WM\\Characters'  and the like.\n\tBackupSaves    = 'true' or 'false'\n\t\t\tIf set to true, the game will save in both the game's default\n\t\t\t\tsave folder as well as the folder set here.\n\tPreferDefault    = 'true' or 'false'\n\t\t\tIf set to true, the game will try to use default images\n\t\t\t\tbefore trying to find alternate images from the image tree.\n\t");
            XmlComment xcResolution = xmldoc.CreateComment("\n\tResolution     = the name of your interface folder\n\tWidth          = screen width\n\tHeight         = screen height\n\tScaleWidth     = screen scale width\n\tScaleHeight    = screen scale height\n\t\t\tThe old code of the game scaled all windows down to 800x600\n\t\t\tThis has been fixed but any old interfaces will not display correctly\n\t\t\t\tif the scale width and height are not set to 800x600.\n\tFullScreen     = 'true' or 'false'\n\tListScrollAmount = The number of lines to scroll in List boxes.\n\tTextScrollAmount = The number of lines to scroll in Text boxes.\n\t");
            XmlComment xcInitial = xmldoc.CreateComment("\n\tGold is how much gold you start the game with.\n\tGirlMeet is the %chance you'll meet a girl when walking around town.\n\tGirlsHousePerc and SlaveHousePerc is the default House Percentage for free girls and slave girls.\n\tGirlsKeepTips and GirlsKeepTips is whether they keep tips separate from house percent.\n\tSlavePayOutOfPocket is wether or not slave girls get paid by the player directly for certain jobs\n\t\tie. Cleaning, Advertising, Farming jobs, Film jobs, etc.\n\tAutoUseItems is whether or not the game will try to automatically use\n\t\tthe player's items intelligently on girls each week.\n\t\tThis feature needs more testing.\n\tAutoCombatEquip determines whether girls will automatically equip their best weapon and\n\t\tarmor for combat jobs and also automatically unequip weapon and armor for regular\n\t\tjobs where such gear would be considered inappropriate (i.e. whores-with-swords).\n\t\tSet to \"false\" to disable this feature.\n\n\tTortureTraitWeekMod affects multiplying the duration that they will\n\t\tkeep a temporary trait that they get from being tortured.\n\t\tIt is multiplied by the number of weeks in the dungeon.\n`J` added\t\tIf TortureTraitWeekMod is set to -1 then torture is harsher.\n\t\tThis doubles the chance of injuring the girls and doubles evil gain.\n\t\tDamage is increased by half. It also makes breaking the girls wills permanent.\n\t");
            XmlComment xcIncome = xmldoc.CreateComment("\n\tThese are the numbers that will multiply the money from various sources of income.\n\t\tSo setting \"GirlsWorkBrothel\" to \"0.5\" will reduce the cash your girls generate in the brothel by half.\n\t\tYou can also use numbers >1 to increase income if you are so inclined.\n\t");
            XmlComment xcExpenses = xmldoc.CreateComment("\n\tThese are the multipliers for your expenses.\n\n\tTraining doesn't currently have a cost, so I'm setting it to 1 gold per girl per week\n\t\tand defaulting the multiplier to 0 (so no change by default).\n\tSet it higher and training begins to cost beyond the simple loss of income.\n\n\tActressWages are like training costs:\n\tA per-girl expense nominally 1 gold per girl, but with a default factor of 0,\n\t\tso no change to the current scheme unless you alter that.\n\n\tMakingMovies is the setup cost for a movie:\n\tI'm going to make this 1000 gold per movie, but again, with a zero factor by default.\n\n\tOtherwise, same as above, except you probably want numbers > 1 to make things more expensive here.\n\n\t* not all are used but are retained just in case.\n\t");
            XmlComment xcGambling = xmldoc.CreateComment("\n\tGambling:\n\n\tThe starting %chance for the tables is given by \"Odds\"\n\n\tWins and losses on the tables are calculated as the \"Base\" value\n\t\tplus a random number between 1 and the value of \"Spread\".\n\tIf the house wins, the amount is multiplied by the HouseFactor\n\tIf the customer wins, by the customer factor.\n\n\tSo: if Base = 50 and spread = 100 then the basic amount\n\t\twon or lost per customer would be 50+d100.\n\n\tAs it stands, the default odds are near 50%\n\twhile the payout is 2:1 in favour of the house.\n\tSo by default, the tables are rigged!\n\t");
            XmlComment xcTax = xmldoc.CreateComment("\n\tTaxes:\n\tRate is the rate at which your income is taxed.\n\tMin is the minimum adjusted rate after influence is used to lower the tax rate.\n\tLaundry is the Maximum % of your income that can be Laundered and so escape taxation.\n\t\tSo if you have 100g income, and a 25% laundry rating, then between 1 and 25 gold will go directly into your pocket.\n\t\tThe remaining 75 Gold will be taxed at 6% (assuming no reduction due to political influence)\n\t");
            XmlComment xcPregnancy = xmldoc.CreateComment("\n\tPregnancy:\n\tPlayerChance, CustomerChance and MonsterChance give the odds of her\n\t\tgetting knocked up by the PC, a customer and a monster, respectively\n\tGoodSexFactor is the multiplier for the pregnancy chance if both parties were happy post coitus.\n\tChanceOfGirl is the %chance of any baby being female.\n\tWeeksPregnant and WeeksMonsterP is how long she is pregnant for.\n\tMiscarriageChance and MiscarriageMonster is the weekly percent chance that the pregnancy may fail.\n\tWeeksTillGrown is how long is takes for the baby to grow up to age 18\n\t\tThe magic of the world the game is set in causes children to age much faster.\n\t\tReal world is 936 weeks.\n\tCoolDown is how long before the girl can get pregnant again after giving birth.\n\tAntiPregFailure is the chance that an Anti-Preg Potion fails to work.\n\tMultiBirthChance is the chance of multiple births.\n\t");
            XmlComment xcProstitution = xmldoc.CreateComment("\n\tThese are the base chances of rape occurring in a brothel and streetwalking.\n\t");
            XmlComment xcCatacombs = xmldoc.CreateComment("\n\tCatacombs Settings:\n\tUniqueCatacombs:   Chance to get a Unique Girl when exploring the Catacombs.\n\t\tAfter all Unique Girls have been found, the rest will be random girls.\n\t\n\tWho gets What:\n\t\tThese settings will determine the ratio of Girls to Items to Beasts that they try to come back with.\n\t\tIf the Controls are true, these will determine what they try to get when you send a Girl or Gang into the catacombs.\n\t\tThe numbers entered here are normalized into fractions of 100% by the game.\n\t\tNegative numbers are not allowed and all 0s will set to (100/3)% each.\n\t");
            XmlComment xcSlaveMarket = xmldoc.CreateComment("\n\tSlave Market Settings:\n\tUniqueMarket:   Chance to get a Unique Girl from the Slave Market.\n\t\tAfter all Unique Girls have been found, the rest will be random girls.\n\t\n\tSlavesNewWeekly...:\tThe minimum and maximum number of girls in the slave market each turn.\n\t\tAbsolude minimum of 0 and maximum of 20.\n\t\tIf min is higher than max, they get switched.\n\t");
            XmlComment xcGangs = xmldoc.CreateComment("\n\tGangs:\n\tMaxRecruitList limits the maximum number of recruitable gangs listed for you to hire.\n\t\tWARNING: BE CAREFUL here; the number of recruitable gangs plus the number of potential hired\n\t\t\tgangs must not exceed the number of names stored in HiredGangNames.txt.\n\t\tFor example, with 20 names, you could have a max of 12 recruitables since you have to\n\t\t\taccount for the possible 8 hired gangs.\n\tStartRandom is how many random recruitable gangs are created for you at the start of a new game.\n\tStartBoosted is how many stat-boosted starting gangs are also added.\n\tInitMemberMin and InitMemberMax indicate the number of initial gang members which are in each recruitable gang;\n\t\ta random number between Min and Max is picked.\n\tAddNewWeeklyMin and AddNewWeeklyMax indicate how many new random gangs are added to the recruitable\n\t\tgangs list each week; a random number between Min and Max is picked.\n\tChanceRemoveUnwanted is the %chance each week that each unhired gang in the recruitable list is removed.\n\t");
            XmlComment xcItems = xmldoc.CreateComment("\n\tItems:\n\t*** AutoCombatEquip was moved to Initial for .06. Kept here for .05 and earlier.\n\tColors are assigned to items listed on the item management screen by there rarity.\n\tThey are in RGB hex format, so #000000 is black and #FFFFFF is white.\n\t\tRarityColor0: Common\n\t\tRarityColor1: Appears in shop, 50% chance\n\t\tRarityColor2: Appears in shop, 25% chance\n\t\tRarityColor3: Appears in shop, 5% chance\n\t\tRarityColor4: Appears in catacombs, 15% chance\n\t\tRarityColor5: Only given by scripts\n\t\tRarityColor6: Given by scripts or as objective rewards\n\t\tRarityColor7: Appears in catacombs, 5% chance\n\t\tRarityColor8: Appears in catacombs, 1% chance\n\t");
            XmlComment xcFonts = xmldoc.CreateComment("\n\tFonts:\n\tNormal is the font that the game uses for text.\n\tFixed is for a monospaced font for tabular info but nothing currently uses that.\n\tShowPercent determines whether or not % is placed\n\t\tafter the number for stats and skills on the girl details list.\n\tAntialias determines whether font antialiasing (smoothing) is used.\n\n\tIt's worth leaving these in, since once the XML screen format is stable,\n\t\tit will be possible to set custom fonts for different text elements,\n\t\tjust like designing a web page.\n\tExcept that you'll have to distribute the font with the game or mod\n\t\trather than relying on the viewer to have it pre-installed.\n\t");
            XmlComment xcDebug = xmldoc.CreateComment("\n\tHow much logging is needed?\n\t* They currently don't really work all that much but they will be improved.\n\t");


            xeConfig.AppendChild(xcFolders);
            xeFolders.SetAttribute("Characters", textBox_Config_Folders_Characters.Text);
            xeFolders.SetAttribute("Saves", textBox_Config_Folders_Savegames.Text);
            xeFolders.SetAttribute("DefaultImages", textBox_Config_Folders_Default_Images.Text);
            xeFolders.SetAttribute("Items", textBox_Config_Folders_Items.Text);
            if (checkBox_Config_Folders_Backup_Saves.Checked == true) xeFolders.SetAttribute("BackupSaves", "true");
            else xeFolders.SetAttribute("BackupSaves", "false");
            if (checkBox_Config_Folders_Prefer_Default.Checked == true) xeFolders.SetAttribute("PreferDefault", "true");
            else xeFolders.SetAttribute("PreferDefault", "false");
            xeConfig.AppendChild(xeFolders);

            xeConfig.AppendChild(xcResolution);
            xeResolution.SetAttribute("Resolution", textBox_Config_Resolution.Text);
            xeResolution.SetAttribute("Width", textBox_Config_Screen_Width.Text);
            xeResolution.SetAttribute("Height", textBox_Config_Screen_Height.Text);
            if (textBox_Config_Screen_Width_Scale.Text == "") xeResolution.SetAttribute("ScaleWidth", "800");
            else xeResolution.SetAttribute("ScaleWidth", textBox_Config_Screen_Width_Scale.Text);
            if (textBox_Config_Screen_Height_Scale.Text == "") xeResolution.SetAttribute("ScaleHeight", "600");
            else xeResolution.SetAttribute("ScaleHeight", textBox_Config_Screen_Height_Scale.Text);
            if (checkBox_Config_FullScreen.Checked == true) xeResolution.SetAttribute("FullScreen", "true"); else xeResolution.SetAttribute("FullScreen", "false");
            xeResolution.SetAttribute("ListScrollAmount", textBox_Config_List_Scroll_Amount.Text);
            xeResolution.SetAttribute("TextScrollAmount", textBox_Config_Text_Scroll_Amount.Text);
            xeConfig.AppendChild(xeResolution);

            xeConfig.AppendChild(xcInitial);
            xeInitial.SetAttribute("Gold", textBox_Initial_Gold.Text);
            xeInitial.SetAttribute("GirlMeet", textBox_Config_InitialGirlMeet.Text);
            xeInitial.SetAttribute("GirlsHousePerc", textBox_Config_InitialGirlsHousePerc.Text);    // `J` added
            if (checkBox_Config_GirlsKeepTips.Checked == true) xeInitial.SetAttribute("GirlsKeepTips", "true"); else xeInitial.SetAttribute("GirlsKeepTips", "false");  // `J` added
            xeInitial.SetAttribute("SlaveHousePerc", textBox_Config_InitialSlaveHousePerc.Text);
            if (checkBox_Config_SlavesPayOutOfPocket.Checked == true) xeInitial.SetAttribute("SlavePayOutOfPocket", "true"); else xeInitial.SetAttribute("SlavePayOutOfPocket", "false"); // `J` added
            if (checkBox_Config_SlavesKeepTips.Checked == true) xeInitial.SetAttribute("SlaveKeepTips", "true"); else xeInitial.SetAttribute("SlaveKeepTips", "false"); // `J` added
            xeInitial.SetAttribute("GirlsAccom", comboBox_Config_InitialGirlsAccom.SelectedIndex.ToString());
            xeInitial.SetAttribute("SlaveAccom", comboBox_Config_InitialSlaveAccom.SelectedIndex.ToString());
            if (checkBox_Config_AutoUseItems.Checked == true) xeInitial.SetAttribute("AutoUseItems", "true"); else xeInitial.SetAttribute("AutoUseItems", "false");
            if (checkBox_Config_AutoEquipCombat.Checked == true) xeInitial.SetAttribute("AutoCombatEquip", "true"); else xeInitial.SetAttribute("AutoCombatEquip", "false");    // `J` added
            xeInitial.SetAttribute("TortureTraitWeekMod", textBox_Torture_Mod.Text);    // `J` added
            xeInitial.SetAttribute("HoroscopeType", (comboBox_Horoscope_Type.SelectedIndex + 1).ToString());    // `J` added
            xeConfig.AppendChild(xeInitial);

            xeConfig.AppendChild(xcIncome);
            xeIncome.SetAttribute("ExtortionIncome",        textBox_Config_IncomeExtortion.Text);
            xeIncome.SetAttribute("GirlsWorkBrothel",       textBox_Config_IncomeBrothel.Text);
            xeIncome.SetAttribute("GirlsWorkStreet",        textBox_Config_IncomeStreet.Text);
            xeIncome.SetAttribute("MovieIncome",            textBox_Config_IncomeMovie.Text);
            xeIncome.SetAttribute("StripperIncome",         textBox_Config_IncomeStripper.Text);
            xeIncome.SetAttribute("BarmaidIncome",          textBox_Config_IncomeBarmaid.Text);
            xeIncome.SetAttribute("SlaveSales",             textBox_Config_IncomeSlaveSales.Text);
            xeIncome.SetAttribute("ItemSales",              textBox_Config_IncomeItemSales.Text);
            xeIncome.SetAttribute("ClinicIncome",           textBox_Config_IncomeClinic.Text);
            xeIncome.SetAttribute("ArenaIncome",            textBox_Config_IncomeArena.Text);
            xeIncome.SetAttribute("FarmIncome",             textBox_Config_IncomeFarm.Text);
            xeConfig.AppendChild(xeIncome);

            xeConfig.AppendChild(xcExpenses);
            xeExpenses.SetAttribute("Training",             textBox_Config_ExpenseTraining.Text);
            xeExpenses.SetAttribute("MovieCost",            textBox_Config_ExpenseMovie.Text);
            xeExpenses.SetAttribute("ActressWages",         textBox_Config_ExpenseActress.Text);
            xeExpenses.SetAttribute("GoonWages",            textBox_Config_ExpenseGoon.Text);
            xeExpenses.SetAttribute("MatronWages",          textBox_Config_ExpenseMatron.Text);
            xeExpenses.SetAttribute("StaffWages",           textBox_Config_ExpenseStaff.Text);        // `J` Not used but kept in for legacy
            xeExpenses.SetAttribute("GirlSupport",          textBox_Config_ExpenseGirls.Text);
            xeExpenses.SetAttribute("Consumables",          textBox_Config_ExpenseConsumables.Text);
            xeExpenses.SetAttribute("Items",                textBox_Config_ExpenseItems.Text);
            xeExpenses.SetAttribute("SlavesBought",         textBox_Config_ExpenseSlaves.Text);
            xeExpenses.SetAttribute("BuyBrothel",           textBox_Config_ExpenseBrothelPrice.Text);
            xeExpenses.SetAttribute("BrothelSupport",       textBox_Config_ExpenseBrothel.Text);
            xeExpenses.SetAttribute("BarSupport",           textBox_Config_ExpenseBar.Text);
            xeExpenses.SetAttribute("CasinoSupport",        textBox_Config_ExpenseCasino.Text);
            xeExpenses.SetAttribute("Bribes",               textBox_Config_ExpenseBribes.Text);
            xeExpenses.SetAttribute("Fines",                textBox_Config_ExpenseFines.Text);
            xeExpenses.SetAttribute("Advertising",          textBox_Config_ExpenseAdvertising.Text);
            xeConfig.AppendChild(xeExpenses);

            xeConfig.AppendChild(xcGambling);
            xeGambling.SetAttribute("Odds",                 textBox_Config_GamblingOdds.Text + "%");
            xeGambling.SetAttribute("Base",                 textBox_Config_GamblingBase.Text);
            xeGambling.SetAttribute("Spread",               textBox_Config_GamblingSpread.Text);
            xeGambling.SetAttribute("CustomerFactor",       textBox_Config_GamblingCustomer.Text);
            xeGambling.SetAttribute("HouseFactor",          textBox_Config_GamblingHouse.Text);
            xeConfig.AppendChild(xeGambling);

            xeConfig.AppendChild(xcTax);
            xeTax.SetAttribute("Rate",                      textBox_Config_TaxRate.Text + "%");
            xeTax.SetAttribute("Minimum",                   textBox_Config_TaxMinimum.Text + "%");
            xeTax.SetAttribute("Laundry",                   textBox_Config_TaxLaundry.Text + "%");
            xeConfig.AppendChild(xeTax);

            xeConfig.AppendChild(xcPregnancy);
            xePregnancy.SetAttribute("PlayerChance",        textBox_Config_PregnancyPlayer.Text + "%");
            xePregnancy.SetAttribute("CustomerChance",      textBox_Config_PregnancyCustomer.Text + "%");
            xePregnancy.SetAttribute("MonsterChance",       textBox_Config_PregnancyMonster.Text + "%");
            xePregnancy.SetAttribute("GoodSexFactor",       textBox_Config_PregnancyGoodSex.Text);
            xePregnancy.SetAttribute("ChanceOfGirl",        textBox_Config_PregnancyGirlChance.Text + "%");
            xePregnancy.SetAttribute("WeeksPregnant",       textBox_Config_PregnancyWeeksPregnant.Text);
            xePregnancy.SetAttribute("WeeksMonsterP",       textBox_Config_PregnancyWeeksMonsterP.Text);                // `J` added
            xePregnancy.SetAttribute("MiscarriageChance",   textBox_Config_PregnancyMiscarriageChance.Text + "%");      // `J` added
            xePregnancy.SetAttribute("MiscarriageMonster",  textBox_Config_PregnancyMiscarriageMonster.Text + "%");     // `J` added
            xePregnancy.SetAttribute("WeeksTillGrown",      textBox_Config_PregnancyWeeksTillGrown.Text);
            xePregnancy.SetAttribute("CoolDown",            textBox_Config_PregnancyCoolDown.Text);
            xePregnancy.SetAttribute("AntiPregFailure",     textBox_Config_AntiPregFailure.Text);
            xePregnancy.SetAttribute("MultiBirthChance",    textBox_Config_Multi_Birth_Chance.Text);
            xeConfig.AppendChild(xePregnancy);

            xeConfig.AppendChild(xcProstitution);
            xeProstitution.SetAttribute("RapeBrothel",      textBox_Config_ProstitutionRapeBrothel.Text + "%");
            xeProstitution.SetAttribute("RapeStreet",       textBox_Config_ProstitutionRapeStreets.Text + "%");
            xeConfig.AppendChild(xeProstitution);

            xeConfig.AppendChild(xcCatacombs);
            xeCatacombs.SetAttribute("UniqueCatacombs", textBox_Config_Unique_Catacombs.Text);
            if (checkBox_Config_Control_Girls.Checked == true) xeCatacombs.SetAttribute("ControlGirls", "true"); else xeCatacombs.SetAttribute("ControlGirls", "false");
            if (checkBox_Config_Control_Gangs.Checked == true) xeCatacombs.SetAttribute("ControlGangs", "true"); else xeCatacombs.SetAttribute("ControlGangs", "false");
            int girlgirls = trackBar_Config_Control_GirlsA.Value;
            int girlitems = trackBar_Config_Control_GirlsB.Value - trackBar_Config_Control_GirlsA.Value;
            int girlbeast = 100 - trackBar_Config_Control_GirlsB.Value;
            int ganggirls = trackBar_Config_Control_GangsA.Value;
            int gangitems = trackBar_Config_Control_GangsB.Value - trackBar_Config_Control_GangsA.Value;
            int gangbeast = 100 - trackBar_Config_Control_GangsB.Value;
            xeCatacombs.SetAttribute("GirlGetsGirls", girlgirls.ToString());
            xeCatacombs.SetAttribute("GirlGetsItems", girlitems.ToString());
            xeCatacombs.SetAttribute("GirlGetsBeast", girlbeast.ToString());
            xeCatacombs.SetAttribute("GangGetsGirls", ganggirls.ToString());
            xeCatacombs.SetAttribute("GangGetsItems", gangitems.ToString());
            xeCatacombs.SetAttribute("GangGetsBeast", gangbeast.ToString());
            xeConfig.AppendChild(xeCatacombs);


            xeConfig.AppendChild(xcSlaveMarket);
            xeSlaveMarket.SetAttribute("UniqueMarket", textBox_Config_Unique_Market.Text);
            xeSlaveMarket.SetAttribute("SlavesNewWeeklyMin", trackBar_Config_Market_Min.Value.ToString());
            xeSlaveMarket.SetAttribute("SlavesNewWeeklyMax", trackBar_Config_Market_Max.Value.ToString());
            xeConfig.AppendChild(xeSlaveMarket);

            xeConfig.AppendChild(xcGangs);
            xeGangs.SetAttribute("MaxRecruitList",          textBox_config_MaxRecruitList.Text);
            xeGangs.SetAttribute("StartRandom",             textBox_config_StartRandom.Text);
            xeGangs.SetAttribute("StartBoosted",            textBox_config_StartBoosted.Text);
            xeGangs.SetAttribute("InitMemberMin",           textBox_config_InitMemberMin.Text);
            xeGangs.SetAttribute("InitMemberMax",           textBox_config_InitMemberMax.Text);
            xeGangs.SetAttribute("AddNewWeeklyMin",         textBox_config_AddNewWeeklyMin.Text);
            xeGangs.SetAttribute("AddNewWeeklyMax",         textBox_config_AddNewWeeklyMax.Text);
            xeGangs.SetAttribute("ChanceRemoveUnwanted",    textBox_config_ChanceRemoveUnwanted.Text + "%");
            xeConfig.AppendChild(xeGangs);

            xeConfig.AppendChild(xcItems);
            // `J` AutoCombatEquip was moved to Initial. Kept here for legacy
            if (checkBox_Config_AutoEquipCombat.Checked == true) xeItems.SetAttribute("AutoCombatEquip", "true"); else xeItems.SetAttribute("AutoCombatEquip", "false");    // `J` added
            // `J` Rarity Colors need to be moved to interfaces. Leaving these in as defaults for legacy
            xeItems.SetAttribute("RarityColor0", "#000000");              //
            xeItems.SetAttribute("RarityColor1", "#000066");              //
            xeItems.SetAttribute("RarityColor2", "#0000cc");              //
            xeItems.SetAttribute("RarityColor3", "#0066ff");              //
            xeItems.SetAttribute("RarityColor4", "#8f0000");              //
            xeItems.SetAttribute("RarityColor5", "#00ff00");              //
            xeItems.SetAttribute("RarityColor6", "#008f00");              //
            xeItems.SetAttribute("RarityColor7", "#a00000");              //
            xeItems.SetAttribute("RarityColor8", "#e00000");              //
            xeConfig.AppendChild(xeItems);

            xeConfig.AppendChild(xcFonts);
            xeFonts.SetAttribute("Normal", textBox_Config_FontNormal.Text);
            xeFonts.SetAttribute("Fixed", textBox_config_FontFixed.Text);
            if (checkBox_Config_Antialias.Checked == true) xeFonts.SetAttribute("Antialias", "true"); else xeFonts.SetAttribute("Antialias", "false");
            if (checkBox_Config_Show_Percent.Checked == true) xeFonts.SetAttribute("ShowPercent", "true"); else xeFonts.SetAttribute("ShowPercent", "false");
            xeConfig.AppendChild(xeFonts);

            xeConfig.AppendChild(xcDebug);
            if (checkBox_config_LogAll.Checked == true) xeDebug.SetAttribute("LogAll", "true"); else xeDebug.SetAttribute("LogAll", "false");
            if (checkBox_config_LogGirls.Checked == true) xeDebug.SetAttribute("LogGirls", "true"); else xeDebug.SetAttribute("LogGirls", "false");
            if (checkBox_config_LogRGirls.Checked == true) xeDebug.SetAttribute("LogRGirls", "true"); else xeDebug.SetAttribute("LogRGirls", "false");
            if (checkBox_config_LogGirlFights.Checked == true) xeDebug.SetAttribute("LogGirlFights", "true"); else xeDebug.SetAttribute("LogGirlFights", "false");   // `J` added
            if (checkBox_config_LogItems.Checked == true) xeDebug.SetAttribute("LogItems", "true"); else xeDebug.SetAttribute("LogItems", "false");
            if (checkBox_config_LogFonts.Checked == true) xeDebug.SetAttribute("LogFonts", "true"); else xeDebug.SetAttribute("LogFonts", "false");
            if (checkBox_config_LogTorture.Checked == true) xeDebug.SetAttribute("LogTorture", "true"); else xeDebug.SetAttribute("LogTorture", "false");   // `J` added
            if (checkBox_config_LogDebug.Checked == true) xeDebug.SetAttribute("LogDebug", "true"); else xeDebug.SetAttribute("LogDebug", "false");   // `J` added
            if (checkBox_config_LogExtraDetails.Checked == true) xeDebug.SetAttribute("LogExtraDetails", "true"); else xeDebug.SetAttribute("LogExtraDetails", "false");   // `J` added
            if (checkBox_config_LogShowNumbers.Checked == true) xeDebug.SetAttribute("LogShowNumbers", "true"); else xeDebug.SetAttribute("LogShowNumbers", "false");   // `J` added
            xeConfig.AppendChild(xeDebug);

            xmldoc.AppendChild(xeConfig);

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.NewLineOnAttributes = true;
            settings.IndentChars = "    ";
            XmlWriter xmlwrite = XmlWriter.Create(path, settings);

            xmldoc.Save(xmlwrite);
            xmlwrite.Close();
        }
        //resets to default
        private void ConfigReset()
        {
            textBox_Config_Resolution.Text = "J_1024x768";
            textBox_Config_Screen_Width.Text = "1024";
            textBox_Config_Screen_Height.Text = "768";
            textBox_Config_Screen_Width_Scale.Text = "800";
            textBox_Config_Screen_Height_Scale.Text = "600";
            checkBox_Config_FullScreen.Checked = false;
            textBox_Config_List_Scroll_Amount.Text = "3";
            textBox_Config_Text_Scroll_Amount.Text = "3";

            textBox_Initial_Gold.Text = "4000";
            textBox_Config_InitialGirlMeet.Text = "30";
            textBox_Config_InitialGirlsHousePerc.Text = "60";
            checkBox_Config_GirlsKeepTips.Checked = true;
            textBox_Config_InitialSlaveHousePerc.Text = "100";
            checkBox_Config_SlavesPayOutOfPocket.Checked = true;
            checkBox_Config_SlavesKeepTips.Checked = false;
            comboBox_Config_InitialGirlsAccom.SelectedIndex = 5;
            comboBox_Config_InitialSlaveAccom.SelectedIndex = 1;
            checkBox_Config_AutoUseItems.Checked = false;
            checkBox_Config_AutoEquipCombat.Checked = true;
            textBox_Torture_Mod.Text = "1";
            comboBox_Horoscope_Type.SelectedIndex = 0;

            textBox_Config_IncomeExtortion.Text = "1.0";
            textBox_Config_IncomeBrothel.Text = "1.0";
            textBox_Config_IncomeStreet.Text = "1.0";
            textBox_Config_IncomeMovie.Text = "1.0";
            textBox_Config_IncomeStripper.Text = "1.0";
            textBox_Config_IncomeBarmaid.Text = "1.0";
            textBox_Config_IncomeSlaveSales.Text = "1.0";
            textBox_Config_IncomeItemSales.Text = "1.0";
            textBox_Config_IncomeClinic.Text = "1.0";
            textBox_Config_IncomeArena.Text = "1.0";
            textBox_Config_IncomeFarm.Text = "1.0";

            textBox_Config_ExpenseTraining.Text = "0.0";
            textBox_Config_ExpenseMovie.Text = "0.0";
            textBox_Config_ExpenseActress.Text = "0.0";
            textBox_Config_ExpenseGoon.Text = "1.0";
            textBox_Config_ExpenseMatron.Text = "1.0";
            textBox_Config_ExpenseStaff.Text = "1.0";
            textBox_Config_ExpenseGirls.Text = "1.0";
            textBox_Config_ExpenseConsumables.Text = "1.0";
            textBox_Config_ExpenseItems.Text = "0.5";
            textBox_Config_ExpenseSlaves.Text = "1.0";
            textBox_Config_ExpenseBrothelPrice.Text = "1.0";
            textBox_Config_ExpenseBrothel.Text = "1.0";
            textBox_Config_ExpenseBar.Text = "1.0";
            textBox_Config_ExpenseCasino.Text = "1.0";
            textBox_Config_ExpenseBribes.Text = "1.0";
            textBox_Config_ExpenseFines.Text = "1.0";
            textBox_Config_ExpenseAdvertising.Text = "1.0";

            textBox_Config_GamblingOdds.Text = "49";
            textBox_Config_GamblingBase.Text = "79";
            textBox_Config_GamblingSpread.Text = "400";
            textBox_Config_GamblingCustomer.Text = "0.5";
            textBox_Config_GamblingHouse.Text = "1.0";

            textBox_Config_ProstitutionRapeBrothel.Text = "1";
            textBox_Config_ProstitutionRapeStreets.Text = "5";

            checkBox_Config_Control_Gangs.Checked = true;
            checkBox_Config_Control_Girls.Checked = true;
            trackBar_Config_Control_GirlsA.Value = 34;
            trackBar_Config_Control_GirlsB.Value = 67;
            trackBar_Config_Control_GangsA.Value = 34;
            trackBar_Config_Control_GangsB.Value = 67;
            textBox_Config_Unique_Catacombs.Text = "50";
            Label_Config_Control_Girls_Girls.Text = "Girl: 34%";
            Label_Config_Control_Girls_Items.Text = "Item: 33%";
            Label_Config_Control_Girls_Beast.Text = "Beast: 33%";
            Label_Config_Control_Gangs_Girls.Text = "Girl: 34%";
            Label_Config_Control_Gangs_Items.Text = "Item: 33%";
            Label_Config_Control_Gangs_Beast.Text = "Beast: 33%";

            textBox_Config_Unique_Market.Text = "35";
            trackBar_Config_Market_Min.Value = 5;
            trackBar_Config_Market_Max.Value = 12;

            textBox_Config_PregnancyPlayer.Text = "8";
            textBox_Config_PregnancyCustomer.Text = "8";
            textBox_Config_PregnancyMonster.Text = "8";
            textBox_Config_PregnancyGoodSex.Text = "2.0";
            textBox_Config_PregnancyGirlChance.Text = "50";
            textBox_Config_PregnancyWeeksPregnant.Text = "40";
            textBox_Config_PregnancyWeeksMonsterP.Text = "20";
            textBox_Config_PregnancyMiscarriageChance.Text = "0.1";
            textBox_Config_PregnancyMiscarriageMonster.Text = "1.0";
            textBox_Config_PregnancyWeeksTillGrown.Text = "60";
            textBox_Config_PregnancyCoolDown.Text = "4";
            textBox_Config_AntiPregFailure.Text = "0.0";
            textBox_Config_Multi_Birth_Chance.Text = "2.0";

            textBox_Config_TaxRate.Text = "6";
            textBox_Config_TaxMinimum.Text = "1";
            textBox_Config_TaxLaundry.Text = "25";

            textBox_config_MaxRecruitList.Text = "12";
            textBox_config_StartRandom.Text = "2";
            textBox_config_StartBoosted.Text = "2";
            textBox_config_InitMemberMin.Text = "1";
            textBox_config_InitMemberMax.Text = "10";
            textBox_config_ChanceRemoveUnwanted.Text = "33";
            textBox_config_AddNewWeeklyMin.Text = "0";
            textBox_config_AddNewWeeklyMax.Text = "3";

            textBox_Config_FontNormal.Text = "segoeui.ttf";
            textBox_config_FontFixed.Text = "segoeui.ttf";
            checkBox_Config_Antialias.Checked = true;
            checkBox_Config_Show_Percent.Checked = false;

            checkBox_config_LogAll.Checked = false;
            checkBox_config_LogItems.Checked = false;
            checkBox_config_LogGirls.Checked = false;
            checkBox_config_LogRGirls.Checked = false;
            checkBox_config_LogGirlFights.Checked = false;
            checkBox_config_LogFonts.Checked = false;
            checkBox_config_LogTorture.Checked = false;
            checkBox_config_LogDebug.Checked = false;
            checkBox_config_LogExtraDetails.Checked = false;
            checkBox_config_LogShowNumbers.Checked = false;
            
        }
        private void checkBox_config_LogAll_OthersChanged(object sender, EventArgs e)
        {
            // `J` if LogAll changes but not because some other log changes, set all others to match
            if (sender == checkBox_config_LogAll && !LogOtherCheckBoxes)
            {
                LogAllCheckBoxes = true;
                checkBox_config_LogGirls.Checked =
                checkBox_config_LogRGirls.Checked =
                checkBox_config_LogGirlFights.Checked =
                checkBox_config_LogItems.Checked =
                checkBox_config_LogFonts.Checked =
                checkBox_config_LogTorture.Checked =
                checkBox_config_LogDebug.Checked =
                checkBox_config_LogExtraDetails.Checked =
                checkBox_config_LogShowNumbers.Checked =
                checkBox_config_LogAll.Checked;
                LogAllCheckBoxes = false;
            }
            // `J` if any other log changes but not because LogAll changes, correct LogAll
            if (sender != checkBox_config_LogAll && !LogAllCheckBoxes)
            {
                LogOtherCheckBoxes = true;
                if (
                checkBox_config_LogGirls.Checked &&
                checkBox_config_LogRGirls.Checked &&
                checkBox_config_LogGirlFights.Checked &&
                checkBox_config_LogItems.Checked &&
                checkBox_config_LogFonts.Checked &&
                checkBox_config_LogTorture.Checked &&
                checkBox_config_LogDebug.Checked &&
                checkBox_config_LogExtraDetails.Checked &&
                checkBox_config_LogShowNumbers.Checked)
                    checkBox_config_LogAll.Checked = true;
                else
                    checkBox_config_LogAll.Checked = false;
                LogOtherCheckBoxes = false;
            }
        }

        //*********************************
        //*****   Tooltips Controls   *****
        //*********************************

        //hides or shows checkBox for traits list on girls tab in relationship with currently selectd tab
        private void tabControl1_Selected(object sender, TabControlEventArgs e)
        {
            if (tabControl1.SelectedTab == tabPage1_Girls) checkBox_ToggleTraitTooltips.Visible = true;
            else checkBox_ToggleTraitTooltips.Visible = false;
        }
        //toggles display of tooltips of girls, rgirls and items tooltips
        private void checkBox_ToggleListsTooltips_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_ToggleListsTooltips.Checked == false)
            {
                listBox_GirlsList.MouseMove -= new System.Windows.Forms.MouseEventHandler(listBox_GirlsList_MouseMove);
                listBox_RGirlsList.MouseMove -= new System.Windows.Forms.MouseEventHandler(listBox_RGirlsList_MouseMove);
                listBox_ItemsList.MouseMove -= new System.Windows.Forms.MouseEventHandler(listBox_ItemsList_MouseMove);
                toolTip1.SetToolTip(listBox_GirlsList, "");
                toolTip1.SetToolTip(listBox_RGirlsList, "");
                toolTip1.SetToolTip(listBox_ItemsList, "");
            }
            else
            {
                listBox_GirlsList.MouseMove += new System.Windows.Forms.MouseEventHandler(listBox_GirlsList_MouseMove);
                listBox_RGirlsList.MouseMove += new System.Windows.Forms.MouseEventHandler(listBox_RGirlsList_MouseMove);
                listBox_ItemsList.MouseMove += new System.Windows.Forms.MouseEventHandler(listBox_ItemsList_MouseMove);
            }
        }
        //toggles display of tooltips for traits list on girl tab
        private void checkBox_ToggleTraitTooltips_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_ToggleTraitTooltips.Checked == false)
            {
                //                ListBox_G_Traits.MouseMove -= new System.Windows.Forms.MouseEventHandler(checkedListBox1_MouseMove);
                dataGridView_G_Traits.CellMouseMove -= new System.Windows.Forms.DataGridViewCellMouseEventHandler(DataGridView_G_Traits_MouseMove);
                //                toolTip1.SetToolTip(ListBox_G_Traits, "");
                toolTip1.SetToolTip(dataGridView_G_Traits, "");
            }
            else
            {
                //                ListBox_G_Traits.MouseMove += new System.Windows.Forms.MouseEventHandler(checkedListBox1_MouseMove);
                dataGridView_G_Traits.CellMouseMove += new System.Windows.Forms.DataGridViewCellMouseEventHandler(DataGridView_G_Traits_MouseMove);
            }
        }

        //*************************
        //*****   Girls tab   *****
        //*************************

        //Add girl to girls list
        private void button_G_AddToList_Click(object sender, EventArgs e)
        {
            if (comboBox_SortByType.SelectedItem.ToString() == "All")
            {
                GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));//adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
                SortDataTable(ref GirlsCollection, ref listBox_GirlsList, TBox_G_RealName.Text);		//Resorts the DataTable with girls so list will be in alphabetical order
                StatusLabel1.Text = "Added girl " + "\"" + GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][0] + "\"" + " to list...";	//changes status line text to this
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Normal girls")
            {
                GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));//adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "0", TBox_G_RealName.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Added girl " + "\"" + TBox_G_RealName.Text + "\"" + " to list...";	//changes status line text to this
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Slave girls")
            {
                GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));//adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "1", TBox_G_RealName.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Added girl " + "\"" + TBox_G_RealName.Text + "\"" + " to list...";	//changes status line text to this
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Catacombs girls")
            {
                GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));//adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "2", TBox_G_RealName.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Added girl " + "\"" + TBox_G_RealName.Text + "\"" + " to list...";	//changes status line text to this
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Arena girls")
            {
                GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));//adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "3", TBox_G_RealName.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Added girl " + "\"" + TBox_G_RealName.Text + "\"" + " to list...";	//changes status line text to this
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Your Daughter")
            {
                GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));//adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "4", TBox_G_RealName.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Added girl " + "\"" + TBox_G_RealName.Text + "\"" + " to list...";	//changes status line text to this
            }
        }

        //Overwrites/updates selected girl in girl's list
        private void button_G_Update_Click(object sender, EventArgs e)
        {
            if (comboBox_SortByType.SelectedItem.ToString() == "All")
            {
                GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][0] = CompileGirl().ElementAt(0);	//overwrite selected cell in dataTable with new name
                GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][1] = CompileGirl().ElementAt(1);	//overwrite selected cell in dataTable with new data
                GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][2] = CompileGirl().ElementAt(2);	//overwrite selected cell in dataTable with new data
                SortDataTable(ref GirlsCollection, ref listBox_GirlsList, TBox_G_RealName.Text);				//Resorts the DataTable with girls so list will be in alphabetical order
                StatusLabel1.Text = "Updated item entry No" + (listBox_GirlsList.SelectedIndex + 1).ToString() + " " + "(\"" + GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][0] + "\")...";
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Normal girls")
            {
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	//overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	//that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	//in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "0");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Updated item entry " + "(\"" + TBox_G_RealName.Text + "\")...";
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Slave girls")
            {
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	//overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	//that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	//in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "1");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Updated item entry " + "(\"" + TBox_G_RealName.Text + "\")...";
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Catacombs girls")
            {
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	//overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	//that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	//in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "2");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Updated item entry " + "(\"" + TBox_G_RealName.Text + "\")...";
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Arena girls")
            {
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	//overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	//that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	//in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "3");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Updated item entry " + "(\"" + TBox_G_RealName.Text + "\")...";
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Your Daughter")
            {
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	//overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	//that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	//in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "4");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Updated item entry " + "(\"" + TBox_G_RealName.Text + "\")...";
            }

        }

        /*
         * compile girl method, I moved it from button(s) code so it's easier to manage, 
         * I just need to take care of this one and changes will affect both buttons
        */
        private string[] CompileGirl()
        {
            string[] sGirl = { "", "", "" };//string in which girl data will be compiled
            string nln = "\r\n";			// define next line string
            string sp = " ";				// same with space
            string sTraits = "";			// string to store selected traits in
            string sStats = "";			    // string for stats
            string sSkills = "";			// for skills
            string sSlave = "";			    // ex is slave/isn't slave option, now has one more state
            string sAccomm = "";            // Accommodation
            string sVirgin = "";
            string sGDesc = "";			    // girl description
            string sFName = TBox_G_FirstName.Text;
            string sMName = TBox_G_MiddleName.Text;
            string sLName = TBox_G_SurName.Text;

            int i = 0;
            //            while (i < ListBox_G_Traits.CheckedItems.Count)	//this while loop goes through selected traits and puts them in sTraits string
            //            {
            //                sTraits = sTraits + ListBox_G_Traits.CheckedItems[i].ToString() + nln;		//since every trait is in separate line I need to add carraige return at the end, this is where that "nln" comes in
            //                i++;
            //            }
            int numberoftraits = 0;
            while (i < TraitCollection.Rows.Count)
            {
                string check = TraitCollection.Rows[i]["Checked"].ToString();
                if (check == "true")
                {
                    sTraits = sTraits + TraitCollection.Rows[i]["Name"].ToString() + nln;
                    numberoftraits++;
                }
                i++;
            }

            sGDesc = TBox_G_Desc.Text;				//originally description was pulled directly from desc textbox, but game has some issues if description is empty so I put these two lines, first picks up what's in desc textbox, and second checks if it's emtpy, if it is then it puts - in there so it won't be empty
            if (sGDesc.Length == 0) sGDesc = "-";

            //next two lines are collection of numbers from stats and skills textboxes,and this is where that "sp" comes in handy, it's easier to type sp than " " every time
            sStats = StatsTBox_01.Text + sp + StatsTBox_02.Text + sp + StatsTBox_03.Text + sp + StatsTBox_04.Text + sp + StatsTBox_05.Text + sp + StatsTBox_06.Text + sp + StatsTBox_07.Text + sp + StatsTBox_08.Text + sp + StatsTBox_09.Text + sp + StatsTBox_10.Text + sp + StatsTBox_11.Text + sp + StatsTBox_12.Text + sp + StatsTBox_13.Text + sp + StatsTBox_14.Text + sp + StatsTBox_15.Text + sp + StatsTBox_16.Text + sp + StatsTBox_17.Text + sp + StatsTBox_18.Text + sp + StatsTBox_19.Text + sp + StatsTBox_20.Text + sp + StatsTBox_21.Text + sp + StatsTBox_22.Text + sp + StatsTBox_23.Text + sp + StatsTBox_24.Text + sp + StatsTBox_25.Text + sp + StatsTBox_26.Text
                + sp + StatsTBox_27.Text
                + sp + "0"; // `J` Crazy added NPCLove but it is not going to be editable yet
            sSkills = SkillTBox_01.Text + sp + SkillTBox_02.Text + sp + SkillTBox_03.Text + sp + SkillTBox_04.Text + sp + SkillTBox_05.Text + sp + SkillTBox_06.Text + sp + SkillTBox_07.Text + sp + SkillTBox_08.Text + sp + SkillTBox_09.Text + sp + SkillTBox_10.Text + sp + SkillTBox_11.Text + sp + SkillTBox_12.Text + sp + SkillTBox_13.Text + sp + SkillTBox_14.Text + sp + SkillTBox_15.Text + sp + SkillTBox_16.Text + sp + SkillTBox_17.Text + sp + SkillTBox_18.Text + sp + SkillTBox_19.Text + sp + SkillTBox_20.Text + sp + SkillTBox_21.Text + sp + SkillTBox_22.Text;
            if (comboBox_Girl_Type.SelectedIndex == 1) sSlave = "1";		//if check to see if slave checkbox is checked or not, and to set slave flag accordingly
            else if (comboBox_Girl_Type.SelectedIndex == 2) sSlave = "2";	//maybe I should have simply said "sSlave = comboBox_GirlType.SelectedIndex.ToString();" that would eliminate ifs, but this way it's more obvious, and it's not necessary to have these items in specific order in dropbox (although they are at the moment)
            else if (comboBox_Girl_Type.SelectedIndex == 3) sSlave = "3";    // arena girls
            else if (comboBox_Girl_Type.SelectedIndex == 4) sSlave = "4";    // your daughter
            else sSlave = "0";

            sAccomm = comboBox_Girl_Accomm.SelectedIndex.ToString();

            if (checkBox_G_Virgin.Checked) sVirgin = "1";

            //first element in array is name
            sGirl[0] = TBox_G_RealName.Text;
            //second is girl data, this is where nln comes handy, easier to type nln than "\n\r" every time, and less error prone
            sGirl[1] = sGDesc + nln + numberoftraits + nln + sTraits + "0" + nln + sStats + nln + sSkills + nln + GoldTBox1.Text + nln + sVirgin + nln + sSlave + nln + sAccomm +nln + sFName + nln + sMName + nln + sLName;
            sGirl[2] = sSlave;

            return sGirl;
        }

        //Randomize stats button
        private void button_St_R_Click(object sender, EventArgs e)
        {
            StatsTBox_14.Text = J_Rand(18, 81);	    	//Age
            StatsTBox_01.Text = J_Rand(1, 100);			//Charisma
            StatsTBox_17.Text = J_Rand(1, 100);			//Beauty
            StatsTBox_24.Text = J_Rand(-100, 100);		//Refinement
            StatsTBox_08.Text = J_Rand(1, 100);			//Agility
            StatsTBox_27.Text = J_Rand(1, 100);			//Strength
            StatsTBox_04.Text = J_Rand(1, 100);			//Constitution
            StatsTBox_05.Text = J_Rand(1, 100);			//Intelligence
            StatsTBox_07.Text = J_Rand(1, 100);			//Mana
            StatsTBox_23.Text = J_Rand(-100, 100);		//Morality
            StatsTBox_25.Text = J_Rand(-100, 100);		//Dignity
            StatsTBox_06.Text = J_Rand(1, 100);			//Confidence
            StatsTBox_15.Text = J_Rand(1, 100);			//Obedience
            StatsTBox_16.Text = J_Rand(1, 100);			//Spirit
            StatsTBox_03.Text = J_Rand(1, 100);			//Libido
        }
        //Normalized randomize stats button, this one gives values rounded to 10, except for age
        private void button_St_NR_Click(object sender, EventArgs e)
        {
            StatsTBox_14.Text = J_Rand(18, 81, 18, 6);			//Age
            StatsTBox_01.Text = J_Rand(1, 100, 10, 2);			//Charisma
            StatsTBox_17.Text = J_Rand(1, 100, 10, 2);			//Beauty
            StatsTBox_24.Text = J_Rand(-50, 50, 10, 2);			//Refinement
            StatsTBox_08.Text = J_Rand(1, 100, 10, 2);			//Agility
            StatsTBox_27.Text = J_Rand(1, 100, 10, 2);			//Strength
            StatsTBox_04.Text = J_Rand(1, 100, 10, 2);			//Constitution
            StatsTBox_05.Text = J_Rand(1, 100, 10, 2);	    	//Intelligence
            StatsTBox_07.Text = J_Rand(1, 100, 10, 2);			//Mana
            StatsTBox_23.Text = J_Rand(-20, 20, 10, 2);			//Morality
            StatsTBox_25.Text = J_Rand(-20, 20, 10, 2);			//Dignity
            StatsTBox_06.Text = J_Rand(1, 100, 10, 2);			//Confidence
            StatsTBox_15.Text = J_Rand(1, 100, 10, 2);			//Obedience
            StatsTBox_16.Text = J_Rand(1, 100, 10, 2);			//Spirit
            StatsTBox_03.Text = J_Rand(1, 100, 10, 2);	    	//Libido
        }
        //Conditional randomize stats button, this will have way more fiddling than normal randomize to limit results as to my observances of what these usually are
        private void button_St_CR_Click(object sender, EventArgs e)
        {
            StatsTBox_04.Text = J_Rand(40, 50, 5, 3);   //Constitution
            StatsTBox_14.Text = J_Rand(18, 70, 18, 6);  //Age, I'll limit it from 18 to 70
            StatsTBox_01.Text = J_Rand(20, 50, 5, 3);   //Charisma
            StatsTBox_17.Text = J_Rand(40, 77, 5, 3);   //Beauty
            StatsTBox_24.Text = J_Rand(-50, 50, 5, 3);	//Refinement
            StatsTBox_08.Text = J_Rand(30, 100, 5, 3);  //Agility
            StatsTBox_27.Text = J_Rand(30, 100, 5, 3);  //Strength
            StatsTBox_05.Text = J_Rand(10, 90, 5, 3);   //Intelligence
            StatsTBox_07.Text = J_Rand(0, 20, 5, 3);    //Mana
            StatsTBox_23.Text = J_Rand(-20, 20, 5, 3);	//Morality
            StatsTBox_25.Text = J_Rand(-20, 20, 5, 3);	//Dignity
            StatsTBox_06.Text = J_Rand(20, 90, 5, 3);   //Confidence
            StatsTBox_15.Text = J_Rand(20, 70, 5, 3);   //Obedience
            StatsTBox_16.Text = J_Rand(30, 70, 5, 3);   //Spirit
            StatsTBox_03.Text = J_Rand(10, 60, 5, 3);   //Libido
        }
        private void button_St_Age_Click(object sender, EventArgs e)
        {
            int age = rnd.Next(0, 82);
            if (age < 18) age = rnd.Next(18, 25);
            if (age > 80) age = 100;
            StatsTBox_14.Text = age.ToString();	    	//Age
        }
        private void button_St_Appearance_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(1, 100);			//Charisma
            StatsTBox_17.Text = J_Rand(1, 100);			//Beauty		  
            StatsTBox_24.Text = J_Rand(-100, 100);			//Refinement
        }
        private void button_St_Physical_Click(object sender, EventArgs e)
        {
            StatsTBox_04.Text = J_Rand(1, 100);			//Constitution
            StatsTBox_08.Text = J_Rand(1, 100);			//Agility
            StatsTBox_27.Text = J_Rand(1, 100);			//Strength
        }
        private void button_St_Mental_Click(object sender, EventArgs e)
        {
            StatsTBox_05.Text = J_Rand(1, 100);			//Intelligence
            StatsTBox_07.Text = J_Rand(1, 100);			//Mana
            StatsTBox_23.Text = J_Rand(-100, 100);			//Morality
            StatsTBox_25.Text = J_Rand(-100, 100);			//Dignity
            StatsTBox_06.Text = J_Rand(1, 100);			//Confidence
            StatsTBox_15.Text = J_Rand(1, 100);			//Obedience
            StatsTBox_16.Text = J_Rand(1, 100);			//Spirit
            StatsTBox_03.Text = J_Rand(1, 100);			//Libido
        }

        //Randomize skills button
        private void button_Sk_R_Click(object sender, EventArgs e)
        {
            SkillTBox_02.Text = J_Rand(0, 100);		//Magic
            SkillTBox_08.Text = J_Rand(0, 100);		//Service
            SkillTBox_10.Text = J_Rand(0, 100);		//Combat
            SkillTBox_13.Text = J_Rand(0, 100);		//Medicine
            SkillTBox_14.Text = J_Rand(0, 100);		//Performance
            SkillTBox_16.Text = J_Rand(0, 100);		//Crafting
            SkillTBox_17.Text = J_Rand(0, 100);		//Herbalism
            SkillTBox_18.Text = J_Rand(0, 100);		//Farming
            SkillTBox_19.Text = J_Rand(0, 100);		//Brewing
            SkillTBox_20.Text = J_Rand(0, 100);		//Animal Handling
            SkillTBox_22.Text = J_Rand(0, 100);		//cooking
        }
        //Normalized randomize skills button
        private void button_Sk_NR_Click(object sender, EventArgs e)
        {
            SkillTBox_02.Text = J_Rand(0, 100, 5, 3);		//Magic
            SkillTBox_08.Text = J_Rand(0, 100, 5, 3);		//Service
            SkillTBox_10.Text = J_Rand(0, 100, 5, 3);		//Combat
            SkillTBox_13.Text = J_Rand(0, 100, 5, 3);		//Medicine
            SkillTBox_14.Text = J_Rand(0, 100, 5, 3);		//Performance
            SkillTBox_16.Text = J_Rand(0, 100, 5, 3);		//Crafting
            SkillTBox_17.Text = J_Rand(0, 100, 5, 3);		//Herbalism
            SkillTBox_18.Text = J_Rand(0, 100, 5, 3);		//Farming
            SkillTBox_19.Text = J_Rand(0, 100, 5, 3);		//Brewing
            SkillTBox_20.Text = J_Rand(0, 100, 5, 3);		//Animal Handling
            SkillTBox_22.Text = J_Rand(0, 100, 5, 3);		//cooking
        }
        //Conditional randomize skills button
        private void button_Sk_CR_Click(object sender, EventArgs e)
        {
            SkillTBox_02.Text = J_Rand(0, 50, 5, 3);		//Magic
            SkillTBox_08.Text = J_Rand(0, 50, 5, 3);		//Service
            SkillTBox_10.Text = J_Rand(0, 50, 5, 3);		//Combat
            SkillTBox_13.Text = J_Rand(0, 50, 5, 3);		//Medicine
            SkillTBox_14.Text = J_Rand(0, 50, 5, 3);		//Performance
            SkillTBox_16.Text = J_Rand(0, 50, 5, 3);		//Crafting
            SkillTBox_17.Text = J_Rand(0, 50, 5, 3);		//Herbalism
            SkillTBox_18.Text = J_Rand(0, 50, 5, 3);		//Farming
            SkillTBox_19.Text = J_Rand(0, 50, 5, 3);		//Brewing
            SkillTBox_20.Text = J_Rand(0, 50, 5, 3);		//Animal Handling
            SkillTBox_22.Text = J_Rand(0, 50, 5, 3);		//Cooking
        }
        private void button_Sk_Focus_Magic_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(0, 100);             //Charisma
            StatsTBox_17.Text = J_Rand(0, 100);             //Beauty                      
            StatsTBox_24.Text = J_Rand(-10, 50);			//Refinement
            StatsTBox_08.Text = J_Rand(0, 80);              //Agility
            StatsTBox_27.Text = J_Rand(0, 80);              //Strength
            StatsTBox_04.Text = J_Rand(0, 60);              //Constitution
            StatsTBox_05.Text = J_Rand(50, 100);            //Intelligence
            StatsTBox_07.Text = J_Rand(50, 100);            //Mana
            StatsTBox_23.Text = J_Rand(-40, 40);			//Morality
            StatsTBox_25.Text = J_Rand(-10, 50);			//Dignity
            StatsTBox_06.Text = J_Rand(30, 100);            //Confidence
            StatsTBox_15.Text = J_Rand(0, 80);              //Obedience
            StatsTBox_16.Text = J_Rand(30, 100);            //Spirit
            StatsTBox_03.Text = J_Rand(0, 100);             //Libido
            SkillTBox_02.Text = J_Rand(50, 100);            //Magic
            SkillTBox_08.Text = J_Rand(0, 30);              //Service
            SkillTBox_10.Text = J_Rand(0, 30);              //Combat
            SkillTBox_13.Text = J_Rand(0, 30);              //Medicine
            SkillTBox_14.Text = J_Rand(10, 40);             //Performance
            SkillTBox_16.Text = J_Rand(0, 50);              //Crafting
            SkillTBox_17.Text = J_Rand(15, 75);             //Herbalism
            SkillTBox_18.Text = J_Rand(0, 30);              //Farming
            SkillTBox_19.Text = J_Rand(0, 30);              //Brewing
            SkillTBox_20.Text = J_Rand(0, 30);              //Animal Handling
            SkillTBox_22.Text = J_Rand(0, 30);              //Cooking
        }
        private void button_Sk_Focus_Combat_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(0, 80);              //Charisma
            StatsTBox_17.Text = J_Rand(0, 80);              //Beauty                      
            StatsTBox_24.Text = J_Rand(-40, 40);			//Refinement
            StatsTBox_08.Text = J_Rand(40, 100);            //Agility
            StatsTBox_27.Text = J_Rand(40, 100);            //Strength
            StatsTBox_04.Text = J_Rand(50, 100);            //Constitution
            StatsTBox_05.Text = J_Rand(0, 70);              //Intelligence
            StatsTBox_07.Text = J_Rand(0, 50);              //Mana
            StatsTBox_23.Text = J_Rand(-50, 50);			//Morality
            StatsTBox_25.Text = J_Rand(-10, 50);			//Dignity
            StatsTBox_06.Text = J_Rand(20, 100);            //Confidence
            StatsTBox_15.Text = J_Rand(0, 80);              //Obedience
            StatsTBox_16.Text = J_Rand(20, 100);            //Spirit
            StatsTBox_03.Text = J_Rand(0, 100);             //Libido
            SkillTBox_02.Text = J_Rand(0, 50);              //Magic
            SkillTBox_08.Text = J_Rand(0, 30);              //Service
            SkillTBox_10.Text = J_Rand(50, 100);            //Combat
            SkillTBox_13.Text = J_Rand(0, 30);              //Medicine
            SkillTBox_14.Text = J_Rand(0, 30);              //Performance
            SkillTBox_16.Text = J_Rand(0, 50);              //Crafting
            SkillTBox_17.Text = J_Rand(0, 20);              //Herbalism
            SkillTBox_18.Text = J_Rand(0, 50);              //Farming
            SkillTBox_19.Text = J_Rand(0, 30);              //Brewing
            SkillTBox_20.Text = J_Rand(0, 50);              //Animal Handling
            SkillTBox_22.Text = J_Rand(0, 30);              //Cooking
        }
        private void button_Sk_Focus_Serv_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(0, 100);             //Charisma
            StatsTBox_17.Text = J_Rand(0, 100);             //Beauty                      
            StatsTBox_24.Text = J_Rand(-20, 50);			//Refinement
            StatsTBox_08.Text = J_Rand(0, 80);              //Agility
            StatsTBox_27.Text = J_Rand(0, 80);              //Strength
            StatsTBox_04.Text = J_Rand(0, 50);              //Constitution
            StatsTBox_05.Text = J_Rand(0, 80);              //Intelligence
            StatsTBox_07.Text = J_Rand(0, 40);              //Mana
            StatsTBox_23.Text = J_Rand(-20, 20);			//Morality
            StatsTBox_25.Text = J_Rand(-40, 20);			//Dignity
            StatsTBox_06.Text = J_Rand(0, 80);              //Confidence
            StatsTBox_15.Text = J_Rand(0, 100);             //Obedience
            StatsTBox_16.Text = J_Rand(0, 80);              //Spirit
            StatsTBox_03.Text = J_Rand(0, 100);             //Libido
            SkillTBox_02.Text = J_Rand(0, 20);              //Magic
            SkillTBox_08.Text = J_Rand(50, 100);            //Service
            SkillTBox_10.Text = J_Rand(0, 20);              //Combat
            SkillTBox_13.Text = J_Rand(0, 20);              //Medicine
            SkillTBox_14.Text = J_Rand(0, 40);              //Performance
            SkillTBox_16.Text = J_Rand(0, 100);             //Crafting
            SkillTBox_17.Text = J_Rand(0, 20);              //Herbalism
            SkillTBox_18.Text = J_Rand(0, 100);             //Farming
            SkillTBox_19.Text = J_Rand(0, 100);             //Brewing
            SkillTBox_20.Text = J_Rand(0, 100);             //Animal Handling
            SkillTBox_22.Text = J_Rand(10, 100);            //Cooking

        }
        private void button_Sk_Focus_Medic_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(21, 100);            //Charisma
            StatsTBox_17.Text = J_Rand(0, 100);             //Beauty                      
            StatsTBox_24.Text = J_Rand(0, 50);			//Refinement
            StatsTBox_08.Text = J_Rand(0, 100);             //Agility
            StatsTBox_27.Text = J_Rand(0, 100);             //Strength
            StatsTBox_04.Text = J_Rand(0, 100);             //Constitution
            StatsTBox_05.Text = J_Rand(50, 100);            //Intelligence
            StatsTBox_07.Text = J_Rand(0, 100);             //Mana
            StatsTBox_23.Text = J_Rand(0, 70);			//Morality
            StatsTBox_25.Text = J_Rand(1, 70);			//Dignity
            StatsTBox_06.Text = J_Rand(20, 100);            //Confidence
            StatsTBox_15.Text = J_Rand(0, 80);              //Obedience
            StatsTBox_16.Text = J_Rand(20, 100);            //Spirit
            StatsTBox_03.Text = J_Rand(0, 100);             //Libido
            SkillTBox_02.Text = J_Rand(0, 50);              //Magic
            SkillTBox_08.Text = J_Rand(0, 40);              //Service
            SkillTBox_10.Text = J_Rand(0, 10);              //Combat
            SkillTBox_13.Text = J_Rand(50, 100);            //Medicine
            SkillTBox_14.Text = J_Rand(0, 20);              //Performance
            SkillTBox_16.Text = J_Rand(0, 50);              //Crafting
            SkillTBox_17.Text = J_Rand(0, 100);             //Herbalism
            SkillTBox_18.Text = J_Rand(0, 20);              //Farming
            SkillTBox_19.Text = J_Rand(0, 50);              //Brewing
            SkillTBox_20.Text = J_Rand(0, 50);              //Animal Handling
            SkillTBox_22.Text = J_Rand(10, 60);             //Cooking
        }
        private void button_Sk_Focus_Perf_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(30, 100);            //Charisma
            StatsTBox_17.Text = J_Rand(30, 100);            //Beauty                      
            StatsTBox_24.Text = J_Rand(10, 100);			//Refinement
            StatsTBox_08.Text = J_Rand(30, 100);            //Agility
            StatsTBox_27.Text = J_Rand(30, 100);            //Strength
            StatsTBox_04.Text = J_Rand(30, 100);            //Constitution
            StatsTBox_05.Text = J_Rand(0, 100);             //Intelligence
            StatsTBox_07.Text = J_Rand(0, 100);             //Mana
            StatsTBox_23.Text = J_Rand(-50, 50);			//Morality
            StatsTBox_25.Text = J_Rand(-30, 80);			//Dignity
            StatsTBox_06.Text = J_Rand(30, 100);            //Confidence
            StatsTBox_15.Text = J_Rand(0, 100);             //Obedience
            StatsTBox_16.Text = J_Rand(0, 100);             //Spirit
            StatsTBox_03.Text = J_Rand(0, 100);             //Libido
            SkillTBox_02.Text = J_Rand(0, 20);              //Magic
            SkillTBox_08.Text = J_Rand(0, 20);              //Service
            SkillTBox_10.Text = J_Rand(0, 20);              //Combat
            SkillTBox_13.Text = J_Rand(0, 20);              //Medicine
            SkillTBox_14.Text = J_Rand(50, 100);            //Performance
            SkillTBox_16.Text = J_Rand(0, 80);              //Crafting
            SkillTBox_17.Text = J_Rand(0, 20);              //Herbalism
            SkillTBox_18.Text = J_Rand(0, 20);              //Farming
            SkillTBox_19.Text = J_Rand(0, 30);              //Brewing
            SkillTBox_20.Text = J_Rand(0, 60);              //Animal Handling
            SkillTBox_22.Text = J_Rand(10, 50);             //Cooking
        }
        private void button_Sk_Focus_Vet_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(0, 80);              //Charisma
            StatsTBox_17.Text = J_Rand(0, 100);             //Beauty                      
            StatsTBox_24.Text = J_Rand(-40, 60);			//Refinement
            StatsTBox_08.Text = J_Rand(0, 100);             //Agility
            StatsTBox_27.Text = J_Rand(0, 100);             //Strength
            StatsTBox_04.Text = J_Rand(0, 100);             //Constitution
            StatsTBox_05.Text = J_Rand(40, 100);            //Intelligence
            StatsTBox_07.Text = J_Rand(0, 100);             //Mana
            StatsTBox_23.Text = J_Rand(-20, 70);			//Morality
            StatsTBox_25.Text = J_Rand(-50, 50);			//Dignity
            StatsTBox_06.Text = J_Rand(20, 80);             //Confidence
            StatsTBox_15.Text = J_Rand(0, 80);              //Obedience
            StatsTBox_16.Text = J_Rand(20, 100);            //Spirit
            StatsTBox_03.Text = J_Rand(0, 100);             //Libido
            SkillTBox_02.Text = J_Rand(0, 50);              //Magic
            SkillTBox_08.Text = J_Rand(0, 40);              //Service
            SkillTBox_10.Text = J_Rand(0, 20);              //Combat
            SkillTBox_13.Text = J_Rand(50, 100);            //Medicine
            SkillTBox_14.Text = J_Rand(0, 10);              //Performance
            SkillTBox_16.Text = J_Rand(0, 50);              //Crafting
            SkillTBox_17.Text = J_Rand(0, 100);             //Herbalism
            SkillTBox_18.Text = J_Rand(0, 20);              //Farming
            SkillTBox_19.Text = J_Rand(0, 30);              //Brewing
            SkillTBox_20.Text = J_Rand(50, 100);            //Animal Handling
            SkillTBox_22.Text = J_Rand(10, 60);             //Cooking
        }
        private void button_Sk_Focus_Farmer_Click(object sender, EventArgs e)
        {
            StatsTBox_01.Text = J_Rand(0, 80);              //Charisma
            StatsTBox_17.Text = J_Rand(0, 80);              //Beauty                      
            StatsTBox_24.Text = J_Rand(-100, 30);			//Refinement
            StatsTBox_08.Text = J_Rand(11, 100);            //Agility
            StatsTBox_27.Text = J_Rand(11, 100);            //Strength
            StatsTBox_04.Text = J_Rand(21, 100);            //Constitution
            StatsTBox_05.Text = J_Rand(0, 80);              //Intelligence
            StatsTBox_07.Text = J_Rand(0, 50);              //Mana
            StatsTBox_23.Text = J_Rand(-50, 50);			//Morality
            StatsTBox_25.Text = J_Rand(-30, 50);			//Dignity
            StatsTBox_06.Text = J_Rand(0, 80);              //Confidence
            StatsTBox_15.Text = J_Rand(11, 80);             //Obedience
            StatsTBox_16.Text = J_Rand(10, 90);             //Spirit
            StatsTBox_03.Text = J_Rand(0, 50);              //Libido
            SkillTBox_02.Text = J_Rand(0, 50);              //Magic
            SkillTBox_08.Text = J_Rand(0, 100);             //Service
            SkillTBox_10.Text = J_Rand(0, 40);              //Combat
            SkillTBox_13.Text = J_Rand(0, 20);              //Medicine
            SkillTBox_14.Text = J_Rand(0, 20);              //Performance
            SkillTBox_16.Text = J_Rand(0, 50);              //Crafting
            SkillTBox_17.Text = J_Rand(21, 80);             //Herbalism
            SkillTBox_18.Text = J_Rand(51, 100);            //Farming
            SkillTBox_19.Text = J_Rand(0, 30);              //Brewing
            SkillTBox_20.Text = J_Rand(20, 100);            //Animal Handling
            SkillTBox_22.Text = J_Rand(10, 50);             //Cooking
        }

        //Randomize sex skills button
        private void button_SS_R_Click(object sender, EventArgs e)
        {
            SkillTBox_01.Text = rnd.Next(0, 101).ToString();		//Anal Sex
            SkillTBox_03.Text = rnd.Next(0, 101).ToString();		//BDSM Sex
            SkillTBox_04.Text = rnd.Next(0, 101).ToString();		//Normal Sex
            SkillTBox_05.Text = rnd.Next(0, 101).ToString();		//Bestiality Sex
            SkillTBox_06.Text = rnd.Next(0, 101).ToString();		//Group Sex
            SkillTBox_07.Text = rnd.Next(0, 101).ToString();		//Lesbian Sex
            SkillTBox_09.Text = rnd.Next(0, 101).ToString();		//Stripping Sex
            SkillTBox_11.Text = rnd.Next(0, 101).ToString();		//OralSex
            SkillTBox_12.Text = rnd.Next(0, 101).ToString();		//TittySex
            SkillTBox_15.Text = rnd.Next(0, 101).ToString();		//Handjob
            SkillTBox_21.Text = rnd.Next(0, 101).ToString();		//Footjob
        }
        //Normalized sex randomize skills button
        private void button_SS_NR_Click(object sender, EventArgs e)
        {
            SkillTBox_01.Text = (rnd.Next(0, 21) * 5).ToString();		//Anal Sex
            SkillTBox_03.Text = (rnd.Next(0, 21) * 5).ToString();		//BDSM Sex
            SkillTBox_04.Text = (rnd.Next(0, 21) * 5).ToString();		//Normal Sex
            SkillTBox_05.Text = (rnd.Next(0, 21) * 5).ToString();		//Bestiality Sex
            SkillTBox_06.Text = (rnd.Next(0, 21) * 5).ToString();		//Group Sex
            SkillTBox_07.Text = (rnd.Next(0, 21) * 5).ToString();		//Lesbian Sex
            SkillTBox_09.Text = (rnd.Next(0, 21) * 5).ToString();		//Stripping Sex
            SkillTBox_11.Text = (rnd.Next(0, 21) * 5).ToString();		//OralSex
            SkillTBox_12.Text = (rnd.Next(0, 21) * 5).ToString();		//TittySex
            SkillTBox_15.Text = (rnd.Next(0, 21) * 5).ToString();		//Handjob
            SkillTBox_21.Text = (rnd.Next(0, 21) * 5).ToString();		//Footjob
        }
        //Conditional sex randomize skills button
        private void button_SS_CR_Click(object sender, EventArgs e)
        {
            SkillTBox_01.Text = (rnd.Next(0, 9) * 5).ToString();		//Anal Sex
            SkillTBox_03.Text = (rnd.Next(0, 9) * 5).ToString();		//BDSM Sex
            SkillTBox_04.Text = (rnd.Next(0, 9) * 5).ToString();		//Normal Sex
            SkillTBox_05.Text = (rnd.Next(0, 9) * 5).ToString();		//Bestiality Sex
            SkillTBox_06.Text = (rnd.Next(0, 9) * 5).ToString();		//Group Sex
            SkillTBox_07.Text = (rnd.Next(0, 9) * 5).ToString();		//Lesbian Sex
            SkillTBox_09.Text = (rnd.Next(0, 9) * 5).ToString();		//Stripping Sex
            SkillTBox_11.Text = (rnd.Next(0, 9) * 5).ToString();		//OralSex
            SkillTBox_12.Text = (rnd.Next(0, 9) * 5).ToString();		//TittySex
            SkillTBox_15.Text = (rnd.Next(0, 9) * 5).ToString();		//Handjob
            SkillTBox_21.Text = (rnd.Next(0, 9) * 5).ToString();		//Footjob
        }
        private void button_SS_Virgin_Click(object sender, EventArgs e)
        {
            SkillTBox_01.Text = J_Rand(0, 15, -5, 6);		//Anal Sex
            SkillTBox_03.Text = J_Rand(0, 10, -5, 6);		//BDSM Sex
            SkillTBox_04.Text = J_Rand(0, 5, -5, 6);	    //Normal Sex
            SkillTBox_05.Text = J_Rand(0, 5, -5, 6);		//Bestiality Sex
            SkillTBox_06.Text = J_Rand(0, 5, -5, 6);	    //Group Sex
            SkillTBox_07.Text = J_Rand(0, 20, -5, 6);		//Lesbian Sex
            SkillTBox_09.Text = J_Rand(0, 30, -5, 6);		//Stripping Sex
            SkillTBox_11.Text = J_Rand(0, 15, -5, 6);		//OralSex
            SkillTBox_12.Text = J_Rand(0, 15, -5, 6);		//TittySex
            SkillTBox_15.Text = J_Rand(0, 15, -5, 6);		//Handjob
            SkillTBox_21.Text = J_Rand(0, 15, -5, 6);		//Footjob
        }
        private void button_SS_Les_Click(object sender, EventArgs e)
        {
            SkillTBox_01.Text = J_Rand(0, 30, 0, 6);        //Anal Sex
            SkillTBox_03.Text = J_Rand(0, 50);   	        //BDSM Sex
            SkillTBox_04.Text = J_Rand(0, 30, 0, 6, -5);    //Normal Sex
            SkillTBox_05.Text = J_Rand(0, 30, 0, 6);        //Bestiality Sex
            SkillTBox_06.Text = J_Rand(0, 50, 10, 6);       //Group Sex
            SkillTBox_07.Text = J_Rand(30, 100, 75, 6, +5); //Lesbian Sex
            SkillTBox_09.Text = J_Rand(0, 50);   	        //Stripping Sex
            SkillTBox_11.Text = J_Rand(0, 50);              //OralSex
            SkillTBox_12.Text = J_Rand(0, 50);              //TittySex
            SkillTBox_15.Text = J_Rand(0, 50);              //Handjob
            SkillTBox_21.Text = J_Rand(0, 50);              //Footjob
        }
        private void button_SS_Milf_Click(object sender, EventArgs e)
        {
            SkillTBox_01.Text = J_Rand(0, 50);		//Anal Sex
            SkillTBox_03.Text = J_Rand(0, 50);		//BDSM Sex
            SkillTBox_04.Text = J_Rand(50, 100);	//Normal Sex
            SkillTBox_05.Text = J_Rand(0, 50);		//Bestiality Sex
            SkillTBox_06.Text = J_Rand(0, 50);		//Group Sex
            SkillTBox_07.Text = J_Rand(0, 50);		//Lesbian Sex
            SkillTBox_09.Text = J_Rand(0, 100);		//Stripping Sex
            SkillTBox_11.Text = J_Rand(0, 50);		//OralSex
            SkillTBox_12.Text = J_Rand(0, 50);		//TittySex
            SkillTBox_15.Text = J_Rand(0, 50);		//Handjob
            SkillTBox_21.Text = J_Rand(0, 50);		//Footjob
        }
        private void button_SS_Whore_Click(object sender, EventArgs e)
        {
            SkillTBox_01.Text = J_Rand(30, 101);	//Anal Sex
            SkillTBox_03.Text = J_Rand(30, 101);	//BDSM Sex
            SkillTBox_04.Text = J_Rand(30, 101);	//Normal Sex
            SkillTBox_05.Text = J_Rand(30, 101);	//Bestiality Sex
            SkillTBox_06.Text = J_Rand(30, 101);	//Group Sex
            SkillTBox_07.Text = J_Rand(30, 101);	//Lesbian Sex
            SkillTBox_09.Text = J_Rand(30, 101);	//Stripping Sex
            SkillTBox_11.Text = J_Rand(30, 101);	//OralSex
            SkillTBox_12.Text = J_Rand(30, 101);	//TittySex
            SkillTBox_15.Text = J_Rand(30, 101);	//Handjob
            SkillTBox_21.Text = J_Rand(30, 101);	//Footjob
        }
        private void button_G_UnZero_Sex_Skills_Click(object sender, EventArgs e)
        {
            for (int i = 0; i < sexskills.Length; i++) { sexskills[i] = "0"; } // reset all to zero before checking
            if (SkillTBox_01.TextLength > 0) sexskills[0] = SkillTBox_01.Text;	//Anal Sex
            if (SkillTBox_03.TextLength > 0) sexskills[1] = SkillTBox_03.Text;	//BDSM Sex
            if (SkillTBox_04.TextLength > 0) sexskills[2] = SkillTBox_04.Text;	//Normal Sex
            if (SkillTBox_05.TextLength > 0) sexskills[3] = SkillTBox_05.Text;	//Bestiality Sex
            if (SkillTBox_06.TextLength > 0) sexskills[4] = SkillTBox_06.Text;	//Group Sex
            if (SkillTBox_07.TextLength > 0) sexskills[5] = SkillTBox_07.Text;	//Lesbian Sex
            if (SkillTBox_09.TextLength > 0) sexskills[6] = SkillTBox_09.Text;	//Stripping Sex
            if (SkillTBox_11.TextLength > 0) sexskills[7] = SkillTBox_11.Text;	//OralSex
            if (SkillTBox_12.TextLength > 0) sexskills[8] = SkillTBox_12.Text;	//TittySex
            if (SkillTBox_15.TextLength > 0) sexskills[9] = SkillTBox_15.Text;	//Handjob
            if (SkillTBox_21.TextLength > 0) sexskills[10] = SkillTBox_21.Text;	//Footjob

            int testnum = 0; int testhigh = 0; int testlow = 0; int testaveragediv = 0; int testaveragesum = 0; 
            int testnumzeros = sexskills.Length;
            bool test5round = true; bool test10round = true; string teststring = "";
            for (int i = 0; i < sexskills.Length; i++)
            {
                if (sexskills[i] != "0")  // only test nonzero numbers for now
                {
                    testnumzeros--;
                    testnum = Convert.ToInt32(sexskills[i]);
                    if (test5round)     // check for rounding
                    {
                        if (testnum % 5 != 0) { test5round = false; test10round = false; }
                        else if (testnum % 10 != 0) test10round = false;
                    }
                    // check high and low values
                    if (testhigh == 0 || testhigh < testnum) { testhigh = testnum; }
                    if (testlow == 0 || testlow > testnum) { testlow = testnum; }
                    // prepare to get the average
                    testaveragediv++; testaveragesum += testnum;
                }
            }
            if (testnumzeros == 0) { }        // none were zero so do nothing
            else if (testnumzeros == sexskills.Length)    // all were zero so change to 1-10
            {
                for (int i = 0; i < sexskills.Length; i++)
                {
                    sexskills[i] = J_Rand(1, 10);
                }
            }
            else if (testnumzeros == sexskills.Length-1)    // only 1 number was set so we use that as the central skew
            {
                for (int i = 0; i < sexskills.Length; i++)
                {
                    int low = testlow - 20; if (low < 0) low = 0;
                    int high = testhigh + 20; if (high > 100) high = 100;
                    if (sexskills[i] == "0") sexskills[i] = J_Rand(low, high, testhigh, 6);
                }
            }
            else if (testhigh == testlow)   // if the existing are all the same, set all to the same
            {
                for (int i = 0; i < sexskills.Length; i++)
                {
                    sexskills[i] = testlow.ToString();
                }
            }
            else                // at least one skill was not zero so we do something about it
            {
                double testaverage = (float)testaveragesum / (float)testaveragediv;             // the average of set values
                double testmedian = (((float)testhigh - (float)testlow) / 2) + (float)testlow;  // the average of high and low

                if (test10round) { teststring = "N10"; }
                else if (test5round) { teststring = "N5"; }
                int skewnum = 0; int skewtype = 0;

                if (testmedian < testaverage - testaveragediv * 2)          // skew up
                {
                    skewnum = testhigh;
                    skewtype = 6;
                }
                else if (testmedian > testaverage + testaveragediv * 2)     // skew down
                {
                    skewnum = testlow;
                    skewtype = 6;

                }
                for (int i = 0; i < sexskills.Length; i++)  // change all that are zero
                {
                    if (sexskills[i] == "0") sexskills[i] = J_Rand(testlow, testhigh, skewnum, skewtype, 0, teststring);
                }
            }

            // finish with setting the skill boxes
            SkillTBox_01.Text = sexskills[0];	//Anal Sex
            SkillTBox_03.Text = sexskills[1];	//BDSM Sex
            SkillTBox_04.Text = sexskills[2];	//Normal Sex
            SkillTBox_05.Text = sexskills[3];	//Bestiality Sex
            SkillTBox_06.Text = sexskills[4];	//Group Sex
            SkillTBox_07.Text = sexskills[5];	//Lesbian Sex
            SkillTBox_09.Text = sexskills[6];	//Stripping Sex
            SkillTBox_11.Text = sexskills[7];	//OralSex
            SkillTBox_12.Text = sexskills[8];	//TittySex
            SkillTBox_15.Text = sexskills[9];	//Handjob
            SkillTBox_21.Text = sexskills[10];	//Footjob
        }

        //Resets values in girl tab, I think this doesn't need any expalantion, it resets all values to starting point, and also deselects any girl on girls list, if any is selected
        private void button_G_Reset_Click(object sender, EventArgs e)
        {
            TBox_G_FirstName.Text = TBox_G_MiddleName.Text = TBox_G_SurName.Text = TBox_G_RealName.Text = TBox_G_Desc.Text = "";
            //            for (int i = 0; i < ListBox_G_Traits.Items.Count; i++)			//loop that unchecks every trait in the list
            //            {
            //                ListBox_G_Traits.SetItemCheckState(i, CheckState.Unchecked);
            //            }
            for (int i = 0; i < TraitCollection.Rows.Count; i++)
            {
                TraitCollection.Rows[i]["Checked"] = "false";
            }

            GoldTBox1.Text = "0";
            StatsTBox_14.Text = "18";
            StatsTBox_12.Text = "60";
            StatsTBox_02.Text = StatsTBox_19.Text = "100";
            StatsTBox_01.Text = StatsTBox_03.Text = StatsTBox_04.Text = StatsTBox_05.Text = StatsTBox_06.Text = StatsTBox_07.Text = StatsTBox_08.Text = StatsTBox_09.Text = StatsTBox_10.Text = StatsTBox_11.Text = StatsTBox_13.Text = StatsTBox_15.Text = StatsTBox_16.Text = StatsTBox_17.Text = StatsTBox_18.Text = StatsTBox_20.Text = StatsTBox_21.Text = StatsTBox_22.Text = StatsTBox_23.Text = StatsTBox_24.Text = StatsTBox_25.Text = StatsTBox_26.Text = StatsTBox_27.Text = "0";
            SkillTBox_01.Text = SkillTBox_02.Text = SkillTBox_03.Text = SkillTBox_04.Text = SkillTBox_05.Text = SkillTBox_06.Text = SkillTBox_07.Text = SkillTBox_08.Text = SkillTBox_09.Text = SkillTBox_10.Text = SkillTBox_11.Text = SkillTBox_12.Text = SkillTBox_13.Text = SkillTBox_14.Text = SkillTBox_15.Text = SkillTBox_16.Text = SkillTBox_17.Text = SkillTBox_18.Text = SkillTBox_19.Text = SkillTBox_20.Text = SkillTBox_21.Text = SkillTBox_22.Text = "0";
            comboBox_Girl_Type.SelectedIndex = 0;
            comboBox_Girl_Accomm.SelectedIndex = 0;
            this.listBox_GirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
            listBox_GirlsList.SelectedItem = null;
            this.listBox_GirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
            checkBox_G_Virgin.Checked = false;

            StatusLabel1.Text = "Reset of girl tab values performed...";
        }

        //button that loads existing girls file, also no need to explain in depth, creates OpenFileDialog named Open, sets it's filter to show only *.girls files by default and passes path of selected file to a function that reads girls from file and fills girls dataTable with values
        private void button_LoadGirls_Click(object sender, EventArgs e)
        {
            OpenFileDialog Open = new OpenFileDialog();
            Open.Filter = "Whore Master girls file|*.girlsx; *.girls|All files|*.*";	//this sets before mentioned filter to show only *.girls files by default
            Open.ShowDialog();
            try
            {
                switch (Path.GetExtension(Open.FileName))
                {
                    case ".girls":
                        LoadGirls(Open.FileName);       //calls LoadGirls function and passes file name with path to it
                        break;
                    case ".girlsx":
                        LoadGirlsXML(Open.FileName);
                        break;
                }
                SortDataTable(ref GirlsCollection, ref listBox_GirlsList);  //sorts the list after it's been loaded, this will be kinda redundant "later" when all girls files have passed through updated WMEditor so they'll already sorted, but since there's no way to know if this is the case on particular file...
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(Open.FileName, err.Message);
                StatusLabel1.Text = "XML Trait load error...";
                MessageBox.Show("File load error in file \n\n" + Open.FileName + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "Open girls(x) file error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }
        //method that parses girls files to fill listbox and DataTable
        private void LoadGirls(string path)
        {
            try																		//concept is same as loading traits, only what it does inside is different
            {
                int x = 0;
                StreamReader Import = new StreamReader(path);
                while (Import.Peek() >= 0)											//again, do this until EoF, it reads line by line of each girl (they are variable in number of lines, but how variable is specified)
                {
                    x++;
                    string name = Import.ReadLine();				//stores line with name in separate string
                    string desc = Import.ReadLine();				//same with description
                    string num = Import.ReadLine();				//next is the number that says how many traits are there (this is why each girl can be different in number of lines from other girls), it could have been imported to int instead of string, but it's used few lines below as string so this way there's one conversion less (there's only one, string to int in for loop, other way it would be one to convert string from file to int so it can be stored as int, and then later int to string to add to dataTable)
                    string temp = "";												//this is where traits are temporarily stored

                    for (int i = 0; i < Convert.ToInt32(num); i++)					//this for loop reads line by line and adds it to temp string, for some reason I didn't use nln thingy as in some previous cases, ergo "\r\n"s
                    {
                        temp = temp + Convert.ToString(Import.ReadLine()) + "\r\n";
                    }

                    string zero = Import.ReadLine();				//this is the line that is currently always "0", maybe it won't be for ever, so better to just parse it as every other line then assume it will always be "0"
                    string data1 = Import.ReadLine();				//line with stats
                    string data2 = Import.ReadLine();				//line with skills
                    string gold = Import.ReadLine();				//gold
                    string girlType = Import.ReadLine();			//type (slave, catacombs...)

                    // `J` adding in to allow the old .girls to add 0 in all stats/skills that they don't have.
                    string[] sStats = data1.Split(' ');
                    string[] sSkills = data2.Split(' ');

                    while (sStats.Length < 28)
                    {
                        data1 += " 0";
                        sStats = data1.Split(' ');
                    }

                    while (sSkills.Length < 23)
                    {
                        data2 += " 0";
                        sSkills = data2.Split(' ');
                    }


                    listBox_GirlsList.Items.Add(name);								//adds girl's name to girls list
                    GirlsCollection.Rows.Add(name, desc + "\r\n" + num + "\r\n" + temp + zero + "\r\n" + data1 + "\r\n" + data2 + "\r\n" + gold + "\r\n" + girlType, girlType);	//adds girls name and data to girls datatable, and GirlType so it can be sorted on that key
                }
                Import.Close();
                StatusLabel1.Text = "Loaded " + x.ToString() + " girls from file...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("File load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "Open error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "Load canceled...";
            }
        }
        //loads girls from XML file
        private void LoadGirlsXML(string path)
        {
            try
            {
                XmlTextReader xmlread = new XmlTextReader(path);				//Creates XmlTextReader instance to open/read XML file
                XmlDocument xmldoc = new XmlDocument();							//Creates XmlDocument instance that will be used to go through XML file
                xmldoc.Load(xmlread);
                XmlNode baseNode = xmldoc.DocumentElement;						//This one isolates one node in our XmlDocument
                int x = 0;														//this is just a counter, it gets incremented for each "girl" read, so at the end we have a number of girls loaded, this is only used to display in status bar how many girls were loaded
                foreach (XmlNode node in baseNode.SelectNodes("/Girls/Girl"))	//Here we tell it what node we want, basically get collection of all Girl nodes in Girls node, in original XML "Girls" is root node, and each girl has her "Girl" node
                {
                    x++;									//here's where we increment our girl counter, getting that number after loading (like geting count of entries in the list) won't work since list may not be empty before this
                    string sName = "";				        //girl name, each girl value has it's attribute within it's node, this is how they're read
                    string sFName = "";
                    string sMName = "";
                    string sLName = "";
                    string sDesc = "";				        //girl description
                    string sData = "";				        //string that will store girl data
                    string sSkills = "";			        //girl skills
                    string sGold = "";				        //starting gold
                    string sVirgin = "";				    //starting Virginity
                    string sStatus = "";			        //girl status
                    string sAccomm = "";			        //girl status
                    ArrayList alTraits = new ArrayList();	//ArrayList to store parsed traits, later on in the code of this method

                    string[] jStats = new string[28];
                    string[] jSkills = new string[22];

                    //now, reason why editor turned out to support conversion to new format is because I made this XML support as a wrapper, data get's read from XML it's stored
                    //in memory in old format, this is what next two lines do, they're that long line with bunch of numbers, internally program works the same as before, only new is this XML wrapper
                    /* `J` load the attributes no matter what order they are in the girlsx file.
                     * If an older girlsx file does not have an attribute, it gets added and set to default
                     * This if loop will make adding new attributes easier in the future.
                    // */
                    //
                    for (int i = 0; i < node.Attributes.Count; i++)
                    {
                        if (node.Attributes[i].Name == "Name") sName = node.Attributes["Name"].Value;
                        if (node.Attributes[i].Name == "FirstName") sFName = node.Attributes["FirstName"].Value;
                        if (node.Attributes[i].Name == "MiddleName") sMName = node.Attributes["MiddleName"].Value;
                        if (node.Attributes[i].Name == "Surname") sLName = node.Attributes["Surname"].Value;

                        if (node.Attributes[i].Name == "Desc") sDesc = node.Attributes["Desc"].Value;
                        if (node.Attributes[i].Name == "Gold") sGold = node.Attributes["Gold"].Value;
                        if (node.Attributes[i].Name == "Virgin") sVirgin = node.Attributes["Virgin"].Value;

                        if (node.Attributes[i].Name == "Charisma") jStats[0] = node.Attributes["Charisma"].Value;
                        if (node.Attributes[i].Name == "Happiness") jStats[1] = node.Attributes["Happiness"].Value;
                        if (node.Attributes[i].Name == "Libido") jStats[2] = node.Attributes["Libido"].Value;
                        if (node.Attributes[i].Name == "Constitution") jStats[3] = node.Attributes["Constitution"].Value;
                        if (node.Attributes[i].Name == "Intelligence") jStats[4] = node.Attributes["Intelligence"].Value;
                        if (node.Attributes[i].Name == "Confidence") jStats[5] = node.Attributes["Confidence"].Value;
                        if (node.Attributes[i].Name == "Mana") jStats[6] = node.Attributes["Mana"].Value;
                        if (node.Attributes[i].Name == "Agility") jStats[7] = node.Attributes["Agility"].Value;
                        if (node.Attributes[i].Name == "Fame") jStats[8] = node.Attributes["Fame"].Value;
                        if (node.Attributes[i].Name == "Level") jStats[9] = node.Attributes["Level"].Value;
                        if (node.Attributes[i].Name == "AskPrice") jStats[10] = node.Attributes["AskPrice"].Value;
                        if (node.Attributes[i].Name == "House") jStats[11] = node.Attributes["House"].Value;
                        if (node.Attributes[i].Name == "Accomm") sAccomm = node.Attributes["Accomm"].Value;
                        if (node.Attributes[i].Name == "Exp") jStats[12] = node.Attributes["Exp"].Value;
                        if (node.Attributes[i].Name == "Age") jStats[13] = node.Attributes["Age"].Value;
                        if (node.Attributes[i].Name == "Obedience") jStats[14] = node.Attributes["Obedience"].Value;
                        if (node.Attributes[i].Name == "Spirit") jStats[15] = node.Attributes["Spirit"].Value;
                        if (node.Attributes[i].Name == "Beauty") jStats[16] = node.Attributes["Beauty"].Value;
                        if (node.Attributes[i].Name == "Tiredness") jStats[17] = node.Attributes["Tiredness"].Value;
                        if (node.Attributes[i].Name == "Health") jStats[18] = node.Attributes["Health"].Value;
                        if (node.Attributes[i].Name == "PCFear") jStats[19] = node.Attributes["PCFear"].Value;
                        if (node.Attributes[i].Name == "PCLove") jStats[20] = node.Attributes["PCLove"].Value;
                        if (node.Attributes[i].Name == "PCHate") jStats[21] = node.Attributes["PCHate"].Value;
                        if (node.Attributes[i].Name == "Morality") jStats[22] = node.Attributes["Morality"].Value;
                        if (node.Attributes[i].Name == "Refinement") jStats[23] = node.Attributes["Refinement"].Value;
                        if (node.Attributes[i].Name == "Dignity") jStats[24] = node.Attributes["Dignity"].Value;
                        if (node.Attributes[i].Name == "Lactation") jStats[25] = node.Attributes["Lactation"].Value;
                        if (node.Attributes[i].Name == "Strength") jStats[26] = node.Attributes["Strength"].Value;
                        if (node.Attributes[i].Name == "NPCLove") jStats[27] = node.Attributes["NPCLove"].Value;

                        if (node.Attributes[i].Name == "Anal") jSkills[0] = node.Attributes["Anal"].Value;
                        if (node.Attributes[i].Name == "Magic") jSkills[1] = node.Attributes["Magic"].Value;
                        if (node.Attributes[i].Name == "BDSM") jSkills[2] = node.Attributes["BDSM"].Value;
                        if (node.Attributes[i].Name == "NormalSex") jSkills[3] = node.Attributes["NormalSex"].Value;
                        if (node.Attributes[i].Name == "Beastiality") jSkills[4] = node.Attributes["Beastiality"].Value;
                        if (node.Attributes[i].Name == "Group") jSkills[5] = node.Attributes["Group"].Value;
                        if (node.Attributes[i].Name == "Lesbian") jSkills[6] = node.Attributes["Lesbian"].Value;
                        if (node.Attributes[i].Name == "Service") jSkills[7] = node.Attributes["Service"].Value;
                        if (node.Attributes[i].Name == "Strip") jSkills[8] = node.Attributes["Strip"].Value;
                        if (node.Attributes[i].Name == "Combat") jSkills[9] = node.Attributes["Combat"].Value;
                        if (node.Attributes[i].Name == "OralSex") jSkills[10] = node.Attributes["OralSex"].Value;
                        if (node.Attributes[i].Name == "TittySex") jSkills[11] = node.Attributes["TittySex"].Value;
                        if (node.Attributes[i].Name == "Medicine") jSkills[12] = node.Attributes["Medicine"].Value;
                        if (node.Attributes[i].Name == "Performance") jSkills[13] = node.Attributes["Performance"].Value;
                        if (node.Attributes[i].Name == "Handjob") jSkills[14] = node.Attributes["Handjob"].Value;
                        if (node.Attributes[i].Name == "Crafting") jSkills[15] = node.Attributes["Crafting"].Value;
                        if (node.Attributes[i].Name == "Herbalism") jSkills[16] = node.Attributes["Herbalism"].Value;
                        if (node.Attributes[i].Name == "Farming") jSkills[17] = node.Attributes["Farming"].Value;
                        if (node.Attributes[i].Name == "Brewing") jSkills[18] = node.Attributes["Brewing"].Value;
                        if (node.Attributes[i].Name == "AnimalHandling") jSkills[19] = node.Attributes["AnimalHandling"].Value;
                        if (node.Attributes[i].Name == "Footjob") jSkills[20] = node.Attributes["Footjob"].Value;
                        if (node.Attributes[i].Name == "Cooking") jSkills[21] = node.Attributes["Cooking"].Value;

                        if (node.Attributes[i].Name == "Status")
                        {
                            switch (node.Attributes["Status"].Value)		//check to read girl status flag (and translate that to old format)
                            {
                                case "Slave": sStatus = "1"; break;
                                case "Catacombs": sStatus = "2"; break;
                                case "Arena": sStatus = "3"; break;
                                case "Your Daughter": sStatus = "4"; break;
                                case "Normal":
                                default: sStatus = "0"; break;
                            }
                        }
                    }

                    // fix names if not set
                    if (sName.Length < 1 && (sFName.Length > 1 || sMName.Length > 1 || sLName.Length < 1))
                    {
                        sName = G_Create_Real_Name(sFName, sMName, sLName);
                    }
                    else if (sName.Length > 0 && sFName.Length < 1 && sMName.Length < 1 && sLName.Length < 1)
                    {
                        string[] names = new string[3];
                        names = G_Divide_Real_Name(sName);
                        sFName = names[0];
                        sMName = names[1];
                        sLName = names[2];
                    }

                    // `J` set defaults if they are not in the girlsx file
                    if (sDesc == null) sDesc = "-";
                    if (sGold == null) sGold = "0";
                    if (sVirgin == null) sVirgin = "0";
                    if (sStatus == null) sStatus = "0";
                    if (sAccomm == null) sAccomm = "0";
                    if (jStats[1] == null) jStats[1] = "100";     // happiness
                    if (jStats[18] == null) jStats[18] = "100";   // health
                    if (jStats[11] == null) jStats[11] = "60";    // house
                    for (int i = 0; i < jStats.Length; i++)       // default all others to 0
                    {
                        sData += (jStats[i] != null) ? jStats[i] + " " : "0 ";
                    }
                    // `J` this sets sex skills if they are not in the girlsx file, non-sex skills are are left as 0
                    if (checkBox_Add_Missing_Sex_Skills_On_Load.Checked)
                    {
                        for (int i = 0; i < 11; i++) { sexskills[i] = null; } // reset all to null before checking
                        sexskills[0] = jSkills[0];	    //Anal Sex
                        sexskills[1] = jSkills[2];	    //BDSM Sex
                        sexskills[2] = jSkills[3];	    //Normal Sex
                        sexskills[3] = jSkills[4];	    //Bestiality Sex
                        sexskills[4] = jSkills[5];	    //Group Sex
                        sexskills[5] = jSkills[6];	    //Lesbian Sex
                        sexskills[6] = jSkills[8];	    //Stripping
                        sexskills[7] = jSkills[10];	    //OralSex
                        sexskills[8] = jSkills[11];	    //TittySex
                        sexskills[9] = jSkills[14];	    //Handjob
                        sexskills[10] = jSkills[20];    //Footjob

                        int testnum = 0; int testnumnulls = 10; int testuniquenums = 0;
                        int testhigh = 0; int testlow = 0; int testaveragediv = 0; int testaveragesum = 0;
                        int[] testuniquelista = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
                        bool test5round = true; bool test10round = true; string teststring = "";
                        for (int i = 0; i < sexskills.Length; i++)
                        {
                            if (sexskills[i] != null)  // only test missing numbers for now
                            {
                                testnumnulls--;
                                testnum = Convert.ToInt32(sexskills[i]);
                                if (test5round)     // check for rounding
                                {
                                    if (testnum % 5 != 0) { test5round = false; test10round = false; }
                                    else if (testnum % 10 != 0) test10round = false;
                                }
                                // check high and low values
                                if (testhigh == 0 || testhigh < testnum) { testhigh = testnum; }
                                if (testlow == 0 || testlow > testnum) { testlow = testnum; }

                                for (int j = 0; j < testuniquelista.Length; j++)
                                {
                                    if (testuniquelista[j] == testnum) break;   // testnum matches a previous number so skip it
                                    if (testuniquelista[j] == -1)               // testnum is not in the list so add it
                                    {
                                        testuniquelista[j] = testnum;
                                        testuniquenums++;
                                        break;
                                    }
                                }
                                // prepare to get the average
                                testaveragediv++; testaveragesum += testnum;
                            }
                        }

                        if (testnumnulls == 11)     // if no numbers are in the girlsx file, we set everything to 0
                        {
                            for (int i = 0; i < sexskills.Length; i++)
                            {
                                sexskills[i] = "0";
                            }
                        }
                        else if (testuniquenums == 1) // if the existing are all the same, fill in the rest
                        {
                            for (int i = 0; i < sexskills.Length; i++)
                            {
                                sexskills[i] = testhigh.ToString();
                            }
                        }
                        // check if it is a pre.05 girlsx and the numbers are similar
                        else if (testnumnulls == 3 && sexskills[7] == null && sexskills[8] == null && sexskills[9] == null && sexskills[10] == null && testuniquenums < 4)
                        {
                            //set oral, hand and foot to group
                            sexskills[7] = sexskills[9] = sexskills[10] = sexskills[4];
                            //set titty to normal
                            sexskills[8] = sexskills[2];
                        }
                        // otherwise randomize the missing
                        else
                        {
                            double testaverage = (float)testaveragesum / (float)testaveragediv;             // the average of set values
                            double testmedian = (((float)testhigh - (float)testlow) / 2) + (float)testlow;  // the average of high and low
                            if (test10round) { teststring = "N10"; } else if (test5round) { teststring = "N5"; }
                            int skewnum = 0; int skewtype = 0;
                            if (testmedian < testaverage - testaveragediv * 2)          // skew up
                            {
                                skewnum = testhigh;
                                skewtype = 6;
                            }
                            else if (testmedian > testaverage + testaveragediv * 2)     // skew down
                            {
                                skewnum = testlow;
                                skewtype = 6;
                            }
                            for (int i = 0; i < 11; i++)  // change all that are missing
                            {
                                if (sexskills[i] == null) sexskills[i] = J_Rand(testlow, testhigh, skewnum, skewtype, 0, teststring);
                            }
                        }
                        // set the numbers
                        jSkills[0] = sexskills[0];  //Anal Sex
                        jSkills[2] = sexskills[1];  //BDSM Sex
                        jSkills[3] = sexskills[2];  //Normal Sex
                        jSkills[4] = sexskills[3];  //Bestiality Sex
                        jSkills[5] = sexskills[4];  //Group Sex
                        jSkills[6] = sexskills[5];  //Lesbian Sex
                        jSkills[8] = sexskills[6];  //Stripping Sex
                        jSkills[10] = sexskills[7];  //OralSex
                        jSkills[11] = sexskills[8];  //TittySex
                        jSkills[14] = sexskills[9];  //Handjob
                        jSkills[20] = sexskills[10]; //Footjob
                    }   // end of if (checkBox_Add_Missing_Sex_Skills_On_Load.Checked)

                    for (int i = 0; i < jSkills.Length; i++)    // default all others to 0
                    {
                        if (jSkills[i] != null)
                            sSkills += jSkills[i] + " ";
                        else sSkills += "0 ";
                    }
                    /*  
                     * next are traits, they're child node of parent "Girl" node, 
                     * simply check if there are any, 
                     * if yes go through all of them to store each one in alTraits ArrayList
                    */
                    if (node.HasChildNodes == true)
                    {
                        for (int y = 0; y < node.ChildNodes.Count; y++)
                        {
                            if (node.ChildNodes[y].NodeType != XmlNodeType.Element) continue;   // `J` only process elements
                            alTraits.Add(node.ChildNodes[y].Attributes["Name"].Value);
                        }
                    }
                    /*  
                     * old format requires that there's a number of traits, this is it. 
                     * This string will store all traits, so first we put number of traits in it
                    */
                    string sTraits = alTraits.Count.ToString();

                    for (int y = 0; y < alTraits.Count; y++)		//and then go through all of them in alTraits and add them to sTraits string. When it's done it'll contain traits in the old format
                    {											    //now that I think about it, this could have been done directly in the part that puts them in alTraits and skip that alTraits completely
                        sTraits = sTraits + "\r\n" + alTraits[y];   //but it works as it is and I'm to lazy to change that now :P
                        if (alTraits[y].ToString() == "Virgin") sVirgin = "1";
                    }

                    //when we get to this part one girl has been read from XML and data collected, now we only need to store that data in DataTable in old format, this is what next line does, the one behind it adds her name to girls list
                    GirlsCollection.Rows.Add(sName, sDesc + "\r\n" + sTraits + "\r\n" + "0" + "\r\n" + sData + "\r\n" + sSkills + "\r\n" + sGold + "\r\n" + sVirgin + "\r\n" + sStatus + "\r\n" + sAccomm + "\r\n" + sFName + "\r\n" + sMName + "\r\n" + sLName, sStatus);
                    listBox_GirlsList.Items.Add(sName);
                }
                xmlread.Close();
                StatusLabel1.Text = "Loaded " + x.ToString() + " girls from XML file...";	//and here's where that "x" with number of girls is used
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("XML load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "XML load error error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "XML load error...";
            }
        }

        //Delete selected girl from the list
        private void button_G_DeleteGirl_Click(object sender, EventArgs e)
        {
            if (comboBox_SortByType.SelectedItem.ToString() == "All")
            {
                try
                {
                    this.listBox_GirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
                    StatusLabel1.Text = "\"" + listBox_GirlsList.SelectedItem.ToString() + "\"" + " deleted...";
                    GirlsCollection.Rows[listBox_GirlsList.SelectedIndex].Delete();				//again, since dataTable and list index are the same it's easy to find which row to delete
                    listBox_GirlsList.Items.RemoveAt(listBox_GirlsList.SelectedIndex);
                    this.listBox_GirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
                }
                catch (Exception err)
                {
                    dataGridView_Bad_Files.Rows.Add(listBox_GirlsList.SelectedIndex, err.Message);

                    MessageBox.Show("It's probably nothing serious, i.e. you pressed Delete button\n\rwhile list was empty or nothing was selected, now for \"official\" error:\n\r\n\r" + err.Message, "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    StatusLabel1.Text = "Delete item error...";
                }
            }
            else					//Since every filtering case uses the same temp DataTable we can use same procedure to remove an entry for every one of these cases, thus just one else
            {
                try
                {
                    this.listBox_GirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
                    StatusLabel1.Text = "\"" + listBox_GirlsList.SelectedItem.ToString() + "\"" + " deleted...";
                    GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]].Delete();				//first we need to delete entry from the original because if we delete temp DataTable entry we loose the way to identify which row in original needs to be deleted
                    GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex].Delete();			//since temp dataTable and list index in filtered case are the same it's easy to find which row to delete
                    listBox_GirlsList.Items.RemoveAt(listBox_GirlsList.SelectedIndex);
                    this.listBox_GirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
                }
                catch (Exception err)
                {
                    dataGridView_Bad_Files.Rows.Add(listBox_GirlsList.SelectedIndex, err.Message);
                    MessageBox.Show("It's probably nothing serious, i.e. you pressed Delete button\n\rwhile list was empty or nothing was selected, now for \"official\" error:\n\r\n\r" + err.Message, "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    StatusLabel1.Text = "Delete item error...";
                }
            }
        }

        //clears girls list
        private void button_GirlClearList_Click(object sender, EventArgs e)
        {
            GirlsCollection.Clear();							//again, nothing special here, just clearing list and related dataTable
            listBox_GirlsList.Items.Clear();
            toolTip1.SetToolTip(listBox_GirlsList, "");
            StatusLabel1.Text = "Girl list cleared...";
        }

        //button that compiles girls data from dataTable and saves it to a file
        private void button_SaveGirls_Click(object sender, EventArgs e)
        {
            Filesave.FileName = "";
            Filesave.Filter = "Whore Master XML girls file|*.girlsx|All files|*.*";
            Filesave.DefaultExt = ".girlsx";
            Filesave.ShowDialog();

            try
            {
                if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
                if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");
                SaveGirlsXML(Filesave.FileName);
                StatusLabel1.Text = "Successfully compiled " + listBox_GirlsList.Items.Count.ToString() + " girls...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(Filesave.FileName, err.Message);
                StatusLabel1.Text = "Save Error.";
            }
        }
        //save girls to XML format
        private void SaveGirlsXML(string path)
        {																//and this is other part of the wrapper, this one reads old format from memory and stores it in XML
            XmlDocument xmldoc = new XmlDocument();						//first we'll create XmlDocument

            XmlElement girls = xmldoc.CreateElement("Girls");			//and create nodes that are used in this document
            XmlElement girl = xmldoc.CreateElement("Girl");
            XmlElement trait = xmldoc.CreateElement("Trait");

            xmldoc.AppendChild(girls);									//"Girls" is the root node so we append it to our xmldoc first

            for (int x = 0; x < GirlsCollection.Rows.Count; x++)			//and now we need to go through each girl, i.e. row in DataTable
            {
                girl = xmldoc.CreateElement("Girl");						//I think that creation of child nodes could be placed here instead of there, but this way code looks more readable, anyway, here's where we create our "Girl" node for each girl

                StringReader sData = new StringReader(GirlsCollection.Rows[x][1].ToString());	//girl data is stored in string that's stored in second column of each row in DataTable, to be able to read it easier we'll access that through StringReader object, it has similar properties as TextReader

                string sName = GirlsCollection.Rows[x][0].ToString();	//girl name is stored in first column of the row so we can get to it directly
                string sDesc = sData.ReadLine();							//girl description is first line of description

                int num = Convert.ToInt32(sData.ReadLine());				//after description we get to traits, first line after description is number of traits
                bool foundvirgintrait = false;
                for (int y = 0; y < num; y++)							//we'll use it to know how many lines need to be read (i.e. how many lines are traits)
                {
                    trait = xmldoc.CreateElement("Trait");				//each trait is stored in separate child node under "Girl" node, so here's where that's done
                    string traitname = sData.ReadLine();
                    trait.SetAttribute("Name", traitname);		//Trait name is attribute in "Trait" node, this is where that attribute get's set
                    girl.AppendChild(trait);								//and after that's done append that child node to "Girl" node, rinse and repeat for every trait
                    if (traitname == "Virgin") foundvirgintrait = true;
                }

                string sZero = sData.ReadLine();				//This line is always 0, it's not used for now, but if need be it's stored anyway, it's just not currently saved to XML (there are no attribute for it)
                string[] sStats = sData.ReadLine().Split(' ');	//line with stats, stored in string array, it's basically space delimited data so it get's splitted right away
                string[] sSkills = sData.ReadLine().Split(' ');	//and again for skills
                string sGold = sData.ReadLine();				//gold
                string sVirgin = sData.ReadLine();				//virgin
                string sStatus = sData.ReadLine();				//status, slave, normal, catacombs, arena, your daughter
                string sAccomm = sData.ReadLine();				//Accommodation
                if (!foundvirgintrait && sVirgin == "1")          // add virgin trait if not already there and Virgin box is checked.
                {
                    trait.SetAttribute("Name", "Virgin");		//Trait name is attribute in "Trait" node, this is where that attribute get's set
                    girl.AppendChild(trait);								//and after that's done append that child node to "Girl" node, rinse and repeat for every trait
                }
                switch (sStatus)											//and here's where it gets "translated" to old XML, on old format it's represented with number, in new with string, so this simple replaces that number with string
                {
                    case "1": sStatus = "Slave"; break;
                    case "2": sStatus = "Catacombs"; break;
                    case "3": sStatus = "Arena"; break;
                    case "4": sStatus = "Your Daughter"; break;
                    case "0":
                    default: sStatus = "Normal"; break;
                }
                string sFName = sData.ReadLine(); if (sFName == null) sFName = "";
                string sMName = sData.ReadLine(); if (sMName == null) sMName = "";
                string sLName = sData.ReadLine(); if (sLName == null) sLName = "";

                if (sName.Length < 1 && (sFName.Length > 1 || sMName.Length > 1 || sLName.Length < 1))
                {
                    sName = G_Create_Real_Name(sFName, sMName, sLName);
                }
                else if (sName.Length > 0 && sFName.Length < 1 && sMName.Length < 1 && sLName.Length < 1)
                {
                    string[] names = new string[3];
                    names = G_Divide_Real_Name(sName);
                    sFName = names[0];
                    sMName = names[1];
                    sLName = names[2];
                }

                //now that we have all that data isolated it should be assigned to their respective attributes in "Girl" node, this is what's done here, bunch of attributes getting assingned their values

                girl.SetAttribute("Name", sName);
                girl.SetAttribute("FirstName", sFName);
                girl.SetAttribute("MiddleName", sMName);
                girl.SetAttribute("Surname", sLName);
                girl.SetAttribute("Desc", sDesc);

                girl.SetAttribute("Level", sStats[9]);
                girl.SetAttribute("Exp", sStats[12]);
                girl.SetAttribute("Age", sStats[13]);
                girl.SetAttribute("Fame", sStats[8]);
                girl.SetAttribute("AskPrice", sStats[10]);
                girl.SetAttribute("House", sStats[11]);
                girl.SetAttribute("Accomm", sAccomm);
                girl.SetAttribute("Health", sStats[18]);
                girl.SetAttribute("Happiness", sStats[1]);
                girl.SetAttribute("Tiredness", sStats[17]);
                girl.SetAttribute("PCLove", sStats[20]);
                girl.SetAttribute("PCFear", sStats[19]);
                girl.SetAttribute("PCHate", sStats[21]);
                girl.SetAttribute("Lactation", sStats[25]);
                girl.SetAttribute("Strength", sStats[26]);
                girl.SetAttribute("NPCLove", sStats[27]);
                
                girl.SetAttribute("Charisma", sStats[0]);
                girl.SetAttribute("Beauty", sStats[16]);
                girl.SetAttribute("Refinement", sStats[23]);
                girl.SetAttribute("Agility", sStats[7]);
                girl.SetAttribute("Constitution", sStats[3]);
                girl.SetAttribute("Intelligence", sStats[4]);
                girl.SetAttribute("Mana", sStats[6]);
                girl.SetAttribute("Morality", sStats[22]);
                girl.SetAttribute("Dignity", sStats[24]);
                girl.SetAttribute("Confidence", sStats[5]);
                girl.SetAttribute("Obedience", sStats[14]);
                girl.SetAttribute("Spirit", sStats[15]);
                girl.SetAttribute("Libido", sStats[2]);

                girl.SetAttribute("Combat", sSkills[9]);
                girl.SetAttribute("Magic", sSkills[1]);
                girl.SetAttribute("Service", sSkills[7]);
                girl.SetAttribute("Medicine", sSkills[12]);
                girl.SetAttribute("Performance", sSkills[13]);
                girl.SetAttribute("Crafting", sSkills[15]);
                girl.SetAttribute("Farming", sSkills[17]);
                girl.SetAttribute("Herbalism", sSkills[16]);
                girl.SetAttribute("Brewing", sSkills[18]);
                girl.SetAttribute("AnimalHandling", sSkills[19]);
                girl.SetAttribute("Cooking", sSkills[21]);

                girl.SetAttribute("NormalSex", sSkills[3]);
                girl.SetAttribute("Anal", sSkills[0]);
                girl.SetAttribute("BDSM", sSkills[2]);
                girl.SetAttribute("Beastiality", sSkills[4]);
                girl.SetAttribute("Lesbian", sSkills[6]);
                girl.SetAttribute("Strip", sSkills[8]);
                girl.SetAttribute("Group", sSkills[5]);
                girl.SetAttribute("OralSex", sSkills[10]);
                girl.SetAttribute("TittySex", sSkills[11]);
                girl.SetAttribute("Handjob", sSkills[14]);
                girl.SetAttribute("Footjob", sSkills[20]);

                girl.SetAttribute("Gold", sGold);
                girl.SetAttribute("Virgin", sVirgin);
                girl.SetAttribute("Status", sStatus);

                girls.AppendChild(girl);								//finaly done (with one girl), all her data has been read and assigned to nodes or attributes, now we only need to "stick" this finished "Girl" child node to root "Girls" node and this is what this does. and then goes at begining of the loop for the next one
            }
            //after it's all done we have our XML, although, only in memory, not stored somewhere safe
            XmlWriterSettings settings = new XmlWriterSettings();	//I've tried few ways of saving this, and this had the nicest output (they were all correct XML wise mind you, but output of this has the "nicest" structure as far as human readability goes
            settings.Indent = true;									//indent every node, otherwise it would be harder to find where each node begins, again, not for computer, for some person looking at outputed XML
            settings.NewLineOnAttributes = true;						//without this each node would be one long line, this puts each attribute in new line
            settings.IndentChars = "    ";								//just a character that'll be used for indenting, \t means tab, so indent is one tab,
            XmlWriter xmlwrite = XmlWriter.Create(path, settings);	//now that settings are complete we can write this file, using path passed from button function, and settings we just made

            xmldoc.Save(xmlwrite);									//now we tell our XmlDocument to save itself to our XmlWriter, this is what finally gives us our file
            xmlwrite.Close();										//now to be all nice and proper we close our file, after all it's finished
        }

        //this happens when you select something from girl list listBox, that invokes SelectedIndexChanged event and values from entry with this index get filled to item tab boxes
        private void listBox_GirlsList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox_SortByType.SelectedItem.ToString() == "All") GirlChanged(ref GirlsCollection);
            else if (comboBox_SortByType.SelectedItem.ToString() == "Normal girls") GirlChanged(ref GirlsCollectionTemp);
            else if (comboBox_SortByType.SelectedItem.ToString() == "Slave girls") GirlChanged(ref GirlsCollectionTemp);
            else if (comboBox_SortByType.SelectedItem.ToString() == "Catacombs girls") GirlChanged(ref GirlsCollectionTemp);
            else if (comboBox_SortByType.SelectedItem.ToString() == "Arena girls") GirlChanged(ref GirlsCollectionTemp);
            else if (comboBox_SortByType.SelectedItem.ToString() == "Your Daughter") GirlChanged(ref GirlsCollectionTemp);
        }
        //to allow the previous event to function with different DataTables I've moved it to separate function that gets called with DataTable as parameter, this was needed because of sorting feature
        private void GirlChanged(ref DataTable dt)
        {
            if (listBox_GirlsList.SelectedIndex < 0) return;
            TBox_G_RealName.Text = dt.Rows[listBox_GirlsList.SelectedIndex][0].ToString();	//what goes on is basically compiling in reverse, name is already isolated so it just needs to be put in it's place
            StringReader data = new StringReader(dt.Rows[listBox_GirlsList.SelectedIndex][1].ToString());	//data is basically chunk of text, as it's in file, so we construct StringReader around that data, and proceed to read it and parse it line by line
            TBox_G_Desc.Text = data.ReadLine();					        //first line is description, just pass it through to it's textbox

            int traitNum = Convert.ToInt32(data.ReadLine());	        //next is number of traits, this isn't entered anywhere, upon compiling it's caluculated anew, it's used for following for loop to read all traits from data

            //            for (int i = 0; i < ListBox_G_Traits.Items.Count; i++)	    //well, not this "for" loop... this "for" loop set's every trait to unchecked state, or else they would get mixed while browsing the girls, or you'd have to click reset button before selecting new girl
            //            {
            //                ListBox_G_Traits.SetItemChecked(i, false);
            //            }
            //
            //            for (int i = 0; i < traitNum; i++)                          //two nested for loops, first one reads trait, second one looks in trait list for that trait, when it finds it it checks it, and then first one loads next one, so this one checks it and so on...
            //            {
            //                string trait = data.ReadLine();
            //                for (int j = 0; j < ListBox_G_Traits.Items.Count; j++)
            //                {
            //                    if (ListBox_G_Traits.Items[j].ToString() == trait)
            //                    {
            //                        ListBox_G_Traits.SetItemChecked(j, true);
            //                        break;
            //                    }
            //                }
            //            }
            for (int i = 0; i < TraitCollection.Rows.Count; i++)
            {
                TraitCollection.Rows[i]["Checked"] = "false";
            }
            for (int i = 0; i < traitNum; i++)
            {
                string trait = data.ReadLine();
                for (int j = 0; j < TraitCollection.Rows.Count; j++)
                {
                    if (TraitCollection.Rows[j]["Name"].ToString() == trait)
                    {
                        TraitCollection.Rows[j]["Checked"] = "true";
                        break;
                    }
                }
            }

            data.ReadLine();														//this line is always 0, it isn't displayed anywhere so we'll just flush it

            char[] separator = { ' ' };											//data in item data is space delimited, so to begin extraction from it we create an array of separator characters
            string[] values = data.ReadLine().Split(separator);					//use Split function to put every value that's "between" spaces to array

            //and now fill stats boxes with values from previous array
            StatsTBox_01.Text = (values.Length > 00) ? values[0] : "0";
            StatsTBox_02.Text = (values.Length > 01) ? values[1] : "0";
            StatsTBox_03.Text = (values.Length > 02) ? values[2] : "0";
            StatsTBox_04.Text = (values.Length > 03) ? values[3] : "0";
            StatsTBox_05.Text = (values.Length > 04) ? values[4] : "0";
            StatsTBox_06.Text = (values.Length > 05) ? values[5] : "0";
            StatsTBox_07.Text = (values.Length > 06) ? values[6] : "0";
            StatsTBox_08.Text = (values.Length > 07) ? values[7] : "0";
            StatsTBox_09.Text = (values.Length > 08) ? values[8] : "0";
            StatsTBox_10.Text = (values.Length > 09) ? values[9] : "0";
            StatsTBox_11.Text = (values.Length > 10) ? values[10] : "0";
            StatsTBox_12.Text = (values.Length > 11) ? values[11] : "0";
            StatsTBox_13.Text = (values.Length > 12) ? values[12] : "0";
            StatsTBox_14.Text = (values.Length > 13) ? values[13] : "0";
            StatsTBox_15.Text = (values.Length > 14) ? values[14] : "0";
            StatsTBox_16.Text = (values.Length > 15) ? values[15] : "0";
            StatsTBox_17.Text = (values.Length > 16) ? values[16] : "0";
            StatsTBox_18.Text = (values.Length > 17) ? values[17] : "0";
            StatsTBox_19.Text = (values.Length > 18) ? values[18] : "0";
            StatsTBox_20.Text = (values.Length > 19) ? values[19] : "0";
            StatsTBox_21.Text = (values.Length > 20) ? values[20] : "0";
            StatsTBox_22.Text = (values.Length > 21) ? values[21] : "0";
            StatsTBox_23.Text = (values.Length > 22) ? values[22] : "0";
            StatsTBox_24.Text = (values.Length > 23) ? values[23] : "0";
            StatsTBox_25.Text = (values.Length > 24) ? values[24] : "0";
            StatsTBox_26.Text = (values.Length > 25) ? values[25] : "0";
            StatsTBox_27.Text = (values.Length > 26) ? values[26] : "0";
            //StatsTBox_28.Text = (values.Length > 27) ? values[27] : "0";  // `J` Crazy added this but it is not going to be editable yet

            //again for skills
            values = data.ReadLine().Split(separator);
            SkillTBox_01.Text = (values.Length > 00) ? values[0] : "0";
            SkillTBox_02.Text = (values.Length > 01) ? values[1] : "0";
            SkillTBox_03.Text = (values.Length > 02) ? values[2] : "0";
            SkillTBox_04.Text = (values.Length > 03) ? values[3] : "0";
            SkillTBox_05.Text = (values.Length > 04) ? values[4] : "0";
            SkillTBox_06.Text = (values.Length > 05) ? values[5] : "0";
            SkillTBox_07.Text = (values.Length > 06) ? values[6] : "0";
            SkillTBox_08.Text = (values.Length > 07) ? values[7] : "0";
            SkillTBox_09.Text = (values.Length > 08) ? values[8] : "0";
            SkillTBox_10.Text = (values.Length > 09) ? values[9] : "0";
            SkillTBox_11.Text = (values.Length > 10) ? values[10] : "0";
            SkillTBox_12.Text = (values.Length > 11) ? values[11] : "0";
            SkillTBox_13.Text = (values.Length > 12) ? values[12] : "0";
            SkillTBox_14.Text = (values.Length > 13) ? values[13] : "0";
            SkillTBox_15.Text = (values.Length > 14) ? values[14] : "0";
            SkillTBox_16.Text = (values.Length > 15) ? values[15] : "0";
            SkillTBox_17.Text = (values.Length > 16) ? values[16] : "0";
            SkillTBox_18.Text = (values.Length > 17) ? values[17] : "0";
            SkillTBox_19.Text = (values.Length > 18) ? values[18] : "0";
            SkillTBox_20.Text = (values.Length > 19) ? values[19] : "0";
            SkillTBox_21.Text = (values.Length > 20) ? values[20] : "0";
            SkillTBox_22.Text = (values.Length > 21) ? values[21] : "0";

            GoldTBox1.Text = data.ReadLine();	//gold
            string sVirgin = data.ReadLine();	//virgin
            checkBox_G_Virgin.Checked = (sVirgin == "1") ? true : false;

            string gtype = data.ReadLine();		//and girl type
            if (Convert.ToInt32(gtype) < 0 || Convert.ToInt32(gtype) > 4)		//Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Normal Girl" if value is out of bounds
            {
                comboBox_Girl_Type.SelectedIndex = 0;
                StatusLabel1.Text = "\"" + TBox_G_RealName.Text + "\"" + " parsed successfully, girl type value was out of bounds, defaulted to \"Normal girl\"...";
            }
            else
            {
                comboBox_Girl_Type.SelectedIndex = Convert.ToInt32(gtype);		//if it's "within bounds" just convert string to number and make that item in droplist selected
                StatusLabel1.Text = "\"" + TBox_G_RealName.Text + "\"" + " parsed successfully...";
            }

            string accomm = data.ReadLine();
            if (accomm == null || accomm=="" || Convert.ToInt32(accomm) < 0 || Convert.ToInt32(accomm) > 9)
            {
                comboBox_Girl_Accomm.SelectedIndex = 0;
            }
            else
            {
                comboBox_Girl_Accomm.SelectedIndex = Convert.ToInt32(accomm);
            }


            TBox_G_FirstName.Text = data.ReadLine();
            TBox_G_MiddleName.Text = data.ReadLine();
            TBox_G_SurName.Text = data.ReadLine();
        }

        //displays tooltip on trait entries on girls tab, something I scrounged from the internet
        private void checkedListBox1_MouseMove(object sender, MouseEventArgs e)
        {
            ListBox listBox = (ListBox)sender;								//creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
            int index = listBox.IndexFromPoint(e.Location);					//e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
            if (index > -1 && index < listBox.Items.Count)					//if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
            {
                string tip = listBox.Items[index].ToString();				//this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
                if (tip != lastTip)											//every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
                {															//also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
                    toolTip1.SetToolTip(listBox, aTraits[index].ToString());	//assigns tooltip at index "index" from atraits arraylist
                    lastTip = tip;											//updates lastTip string
                }
            }
        }

        private void DataGridView_G_Traits_MouseMove(object sender, DataGridViewCellMouseEventArgs e)
        {

            if (checkBox_ToggleTraitTooltips.Checked && e.RowIndex > -1 && e.RowIndex < dataGridView_G_Traits.RowCount)
            {
                string tip = dataGridView_G_Traits.Rows[e.RowIndex].Cells["Desc"].Value.ToString();
                if (tip != lastTip)
                {
                    toolTip1.SetToolTip(dataGridView_G_Traits, dataGridView_G_Traits.Rows[e.RowIndex].Cells["Desc"].Value.ToString());
                    lastTip = tip;
                }
            }
        }


        //displays tooltips with girl descriptions on girls list, same as above with traits
        private void listBox_GirlsList_MouseMove(object sender, MouseEventArgs e)
        {
            ListBox listBox = (ListBox)sender;								//creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
            int index = listBox.IndexFromPoint(e.Location);					//e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
            if (index > -1 && index < listBox.Items.Count)					//if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
            {
                string tip = listBox.Items[index].ToString();				//this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
                if (tip != lastTip)											//every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
                {															//also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
                    toolTip1.SetToolTip(listBox, new StringReader(GirlsCollection.Rows[index][1].ToString()).ReadLine());	//this is the only difference from previous tooltip function. Description is stored in dataTable at [index][1], along with other data, so to get it out we first need to identify coordinates (that's what [index][1] is for), then construct StringReader from that long string, and lastly read first line from that stream, this is description. Luckily it can be fitted in one line :P
                    lastTip = tip;											//updates lastTip string
                }
            }
        }

        //dropbox to select filtering by type, simply sends different parameters for different cases of drop boxs, except in first case, it just performs regular sort to repopulate the list
        private void comboBox_SortByType_SelectedIndexChanged(object sender, EventArgs e)
        {
            switch (comboBox_SortByType.SelectedItem.ToString())
            {
                case "Normal girls": FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "0"); break;
                case "Slave girls": FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "1"); break;
                case "Catacombs girls": FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "2"); break;
                case "Arena girls": FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "3"); break;
                case "Your Daughter": FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "4"); break;
                case "All":
                default: SortDataTable(ref GirlsCollection, ref listBox_GirlsList); break;
            }
        }
        //this is filtering function that previous event uses, I'll probably upgrade it to filter everything needed
        private void FilterGirls(DataTable dt, ref DataTable tempDT, ref ListBox lb, string sFilter)
        {
            tempDT.Reset();											//resets temp DataTable
            tempDT = dt.Clone();										//copies the structure from original DataTable

            for (int x = 0; x < dt.Rows.Count; x++) tempDT.ImportRow(dt.Rows[x]);	//this copies content from original DataTable
            tempDT.Columns.Add("ID", typeof(int));					//adds a column to temp DataTable, this is where link with original DataTable will be stored
            for (int x = 0; x < tempDT.Rows.Count; x++)				//at this moment temp DataTable is still exact copy of original, so we fill new ID column with number that represents current row number
            {														//after we filter it they will remain with current values that will tell us what line in original DataTable needs to be updated
                tempDT.Rows[x][3] = x;
            }
            DataView v = tempDT.DefaultView;							//rest is almost the same as with normal sort, create DataView from our DataTable, this time temp DataTable that will get gutted :P, so our original doesn't loose any data
            v.RowFilter = "TypeFilter = '" + sFilter + "'";			//this is the only real change from normal sort, we simply say by which column it needs to filter and by what value, At this moment I've adapted other functions to put type in TypeFilter, actual value it filters comes from where this function is called
            v.Sort = "Name ASC";										//sort this DataView in ascending order using "Name" column as key
            tempDT = v.ToTable();									//apply this sorted view to our original DataTable

            lb.Items.Clear();										//empty listbBox from entries it has

            for (int x = 0; x < tempDT.Rows.Count; x++)				//go through all records in DataTable and add names to listBox so our index sync works again
            {
                lb.Items.Add(tempDT.Rows[x][0].ToString());
            }
        }

        //same as above, but selects an entry after it's done (used for add to list, and update on list buttons)
        private void FilterGirls(DataTable dt, ref DataTable tempDT, ref ListBox lb, string sFilter, string Name)
        {
            tempDT.Reset();
            tempDT = dt.Clone();

            for (int x = 0; x < dt.Rows.Count; x++) tempDT.ImportRow(dt.Rows[x]);

            tempDT.Columns.Add("ID", typeof(int));

            for (int x = 0; x < tempDT.Rows.Count; x++)
            {
                tempDT.Rows[x][3] = x;
            }

            DataView v = tempDT.DefaultView;
            v.RowFilter = "TypeFilter = '" + sFilter + "'";
            v.Sort = "Name ASC";
            tempDT = v.ToTable();

            lb.Items.Clear();

            for (int x = 0; x < tempDT.Rows.Count; x++)
            {
                lb.Items.Add(tempDT.Rows[x][0].ToString());
            }

            lb.SelectedItem = Name;
        }

        //until now, most ideas and concepts have been described, so to cut it a little shorter I'll just comment on variables, what is stored in which one

        //*********************************
        //*****   Random girls tab	*****
        //*********************************

        //transfer traits to datagridview to add percentages
        private void confirmTraitsButton1_Click(object sender, EventArgs e)
        {
            rgTable.Rows.Add(comboBox_RGTraits.SelectedItem, textBox_RGTraitValue.Text);
        }
        //remove selected trait
        private void button_RGRemoveTrait_Click(object sender, EventArgs e)
        {
            try
            {
                rgTable.Rows[dataGridView1.CurrentRow.Index].Delete();
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(dataGridView1.CurrentRow.Index, err.Message);
                StatusLabel1.Text = "Error...";
                MessageBox.Show("Nothing is selected or list is empty.", "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }
        //Add random girl to output tab button
        private void compileSRGButton1_Click(object sender, EventArgs e)
        {
            RGirlsCollection.Rows.Add(CompileRandomGirl().ElementAt(0), CompileRandomGirl().ElementAt(1));
            SortDataTable(ref RGirlsCollection, ref listBox_RGirlsList, nameRTBox1.Text);
            StatusLabel1.Text = "Added random girl " + "\"" + RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][0] + "\"" + " to list...";
        }
        //Add more than one random girl to output tab button
        private void compileMRGButton1_Click(object sender, EventArgs e)
        {
            RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][0] = CompileRandomGirl().ElementAt(0);
            RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][1] = CompileRandomGirl().ElementAt(1);
            SortDataTable(ref RGirlsCollection, ref listBox_RGirlsList, nameRTBox1.Text);
            StatusLabel1.Text = "Updated item entry No" + (listBox_RGirlsList.SelectedIndex + 1).ToString() + " " + "(\"" + RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][0] + "\")...";
        }

        //compile random girl method, moved it from button(s) code so it's easier to manage, I just need to take care of this one and changes will affect both buttons
        private string[] CompileRandomGirl()
        {
            PerformValidate();
            string[] sRGirl = { "", "" };	//as with girls, this is string array that will store name and data
            string nln = "\r\n";
            string sp = " ";
            string sRTraits = "";			//traits for random girls
            string sRStatsMin = "";			//line with minimum value for stats
            string sRStatsMax = "";			//line with maximum value for stats
            string sRSkillsMin = "";			//minimum skills
            string sRSkillsMax = "";			//maximum skills
            string sHuman_Check = "";				//this was previously unsued line9, now it shows if girl is human or not
            string sCatacomb_Check = "";				//line 10, manual says this should be 0, but it seems it's catacombs flag, although currently it doesn't do anything
            string sRGDesc;					//random girl description

            if (checkBox_RGHuman.Checked == true) sHuman_Check = "1";
            else sHuman_Check = "0";

            if (checkBox_RGCatacombs.Checked == true) sCatacomb_Check = "1";
            else sCatacomb_Check = "0";

            int i = 0;
            while (i < dataGridView1.Rows.Count)
            {
                sRTraits = sRTraits + nln + dataGridView1.Rows[i].Cells[0].Value.ToString() + nln + dataGridView1.Rows[i].Cells[1].Value.ToString();
                i++;
            }

            sRStatsMin = StatRGMinTBox1.Text + sp + StatRGMinTBox2.Text + sp + StatRGMinTBox3.Text + sp + StatRGMinTBox4.Text + sp + StatRGMinTBox5.Text + sp + StatRGMinTBox6.Text + sp + StatRGMinTBox7.Text + sp + StatRGMinTBox8.Text + sp + StatRGMinTBox9.Text + sp + StatRGMinTBox10.Text + sp + StatRGMinTBox11.Text + sp + StatRGMinTBox12.Text + sp + "0" + sp + StatRGMinTBox14.Text + sp + StatRGMinTBox15.Text + sp + StatRGMinTBox16.Text + sp + StatRGMinTBox17.Text + sp + StatRGMinTBox18.Text + sp + StatRGMinTBox19.Text + sp + StatRGMinTBox20.Text + sp + StatRGMinTBox21.Text + sp + StatRGMinTBox22.Text + sp + StatRGMinTBox23.Text + sp + StatRGMinTBox24.Text + sp + StatRGMinTBox25.Text + sp + StatRGMinTBox26.Text + sp + StatRGMinTBox27.Text;
            sRStatsMax = StatRGMaxTBox1.Text + sp + StatRGMaxTBox2.Text + sp + StatRGMaxTBox3.Text + sp + StatRGMaxTBox4.Text + sp + StatRGMaxTBox5.Text + sp + StatRGMaxTBox6.Text + sp + StatRGMaxTBox7.Text + sp + StatRGMaxTBox8.Text + sp + StatRGMaxTBox9.Text + sp + StatRGMaxTBox10.Text + sp + StatRGMaxTBox11.Text + sp + StatRGMaxTBox12.Text + sp + "0" + sp + StatRGMaxTBox14.Text + sp + StatRGMaxTBox15.Text + sp + StatRGMaxTBox16.Text + sp + StatRGMaxTBox17.Text + sp + StatRGMaxTBox18.Text + sp + StatRGMaxTBox19.Text + sp + StatRGMaxTBox20.Text + sp + StatRGMaxTBox21.Text + sp + StatRGMaxTBox22.Text + sp + StatRGMaxTBox23.Text + sp + StatRGMaxTBox24.Text + sp + StatRGMaxTBox25.Text + sp + StatRGMaxTBox26.Text + sp + StatRGMaxTBox27.Text;

            sRSkillsMin = SkillRGMinTBox1.Text + sp + SkillRGMinTBox2.Text + sp + SkillRGMinTBox3.Text + sp + SkillRGMinTBox4.Text + sp + SkillRGMinTBox5.Text + sp + SkillRGMinTBox6.Text + sp + SkillRGMinTBox7.Text + sp + SkillRGMinTBox8.Text + sp + SkillRGMinTBox9.Text + sp + SkillRGMinTBox10.Text + sp + SkillRGMinTBox11.Text + sp + SkillRGMinTBox12.Text + sp + SkillRGMinTBox13.Text + sp + SkillRGMinTBox14.Text + sp + SkillRGMinTBox15.Text + sp + SkillRGMinTBox16.Text + sp + SkillRGMinTBox17.Text + sp + SkillRGMinTBox18.Text + sp + SkillRGMinTBox19.Text + sp + SkillRGMinTBox20.Text + sp + SkillRGMinTBox21.Text + sp + SkillRGMinTBox22.Text;
            sRSkillsMax = SkillRGMaxTBox1.Text + sp + SkillRGMaxTBox2.Text + sp + SkillRGMaxTBox3.Text + sp + SkillRGMaxTBox4.Text + sp + SkillRGMaxTBox5.Text + sp + SkillRGMaxTBox6.Text + sp + SkillRGMaxTBox7.Text + sp + SkillRGMaxTBox8.Text + sp + SkillRGMaxTBox9.Text + sp + SkillRGMaxTBox10.Text + sp + SkillRGMaxTBox11.Text + sp + SkillRGMaxTBox12.Text + sp + SkillRGMaxTBox13.Text + sp + SkillRGMaxTBox14.Text + sp + SkillRGMaxTBox15.Text + sp + SkillRGMaxTBox16.Text + sp + SkillRGMaxTBox17.Text + sp + SkillRGMaxTBox18.Text + sp + SkillRGMaxTBox19.Text + sp + SkillRGMaxTBox20.Text + sp + SkillRGMaxTBox21.Text + sp + SkillRGMaxTBox22.Text;

            if (descRTBox1.Text.Length == 0) sRGDesc = "-";
            else sRGDesc = descRTBox1.Text;

            sRGirl[0] = nameRTBox1.Text;
            sRGirl[1] = sRGDesc + nln + sRStatsMin + nln + sRStatsMax + nln + sRSkillsMin + nln + sRSkillsMax + nln + GoldRMinTBox1.Text + nln + GoldRMaxTBox1.Text + nln + sHuman_Check + nln + sCatacomb_Check + nln + dataGridView1.Rows.Count + sRTraits;

            return sRGirl;
        }

        //Function to check if min box value is really smaller than max box value, just to be sure that values entered are indeed min and max
        private void ValidateRG(TextBox min, TextBox max)
        {
            if (min.Text == "") min.Text = "0";
            if (max.Text == "") max.Text = "0";
            if (Convert.ToInt32(min.Text) != 0 && Convert.ToInt32(max.Text) == 0) min.Text = "0";
            if (Convert.ToInt32(min.Text) > Convert.ToInt32(max.Text)) min.Text = (Convert.ToInt32(max.Text) - 1).ToString();
        }
        //Check for every pair of boxes under one roof
        private void PerformValidate()
        {
            ValidateRG(StatRGMinTBox1, StatRGMaxTBox1);
            ValidateRG(StatRGMinTBox2, StatRGMaxTBox2);
            ValidateRG(StatRGMinTBox3, StatRGMaxTBox3);
            ValidateRG(StatRGMinTBox4, StatRGMaxTBox4);
            ValidateRG(StatRGMinTBox5, StatRGMaxTBox5);
            ValidateRG(StatRGMinTBox6, StatRGMaxTBox6);
            ValidateRG(StatRGMinTBox7, StatRGMaxTBox7);
            ValidateRG(StatRGMinTBox8, StatRGMaxTBox8);
            ValidateRG(StatRGMinTBox9, StatRGMaxTBox9);
            ValidateRG(StatRGMinTBox10, StatRGMaxTBox10);
            ValidateRG(StatRGMinTBox11, StatRGMaxTBox11);
            ValidateRG(StatRGMinTBox12, StatRGMaxTBox12);
            ValidateRG(StatRGMinTBox13, StatRGMaxTBox13);
            ValidateRG(StatRGMinTBox14, StatRGMaxTBox14);
            ValidateRG(StatRGMinTBox15, StatRGMaxTBox15);
            ValidateRG(StatRGMinTBox16, StatRGMaxTBox16);
            ValidateRG(StatRGMinTBox17, StatRGMaxTBox17);
            ValidateRG(StatRGMinTBox18, StatRGMaxTBox18);
            ValidateRG(StatRGMinTBox19, StatRGMaxTBox19);
            ValidateRG(StatRGMinTBox20, StatRGMaxTBox20);
            ValidateRG(StatRGMinTBox21, StatRGMaxTBox21);
            ValidateRG(StatRGMinTBox22, StatRGMaxTBox22);
            ValidateRG(StatRGMinTBox23, StatRGMaxTBox23);
            ValidateRG(StatRGMinTBox24, StatRGMaxTBox24);
            ValidateRG(StatRGMinTBox25, StatRGMaxTBox25);
            ValidateRG(StatRGMinTBox26, StatRGMaxTBox26);
            ValidateRG(StatRGMinTBox27, StatRGMaxTBox27);

            ValidateRG(SkillRGMinTBox1, SkillRGMaxTBox1);
            ValidateRG(SkillRGMinTBox2, SkillRGMaxTBox2);
            ValidateRG(SkillRGMinTBox3, SkillRGMaxTBox3);
            ValidateRG(SkillRGMinTBox4, SkillRGMaxTBox4);
            ValidateRG(SkillRGMinTBox5, SkillRGMaxTBox5);
            ValidateRG(SkillRGMinTBox6, SkillRGMaxTBox6);
            ValidateRG(SkillRGMinTBox7, SkillRGMaxTBox7);
            ValidateRG(SkillRGMinTBox8, SkillRGMaxTBox8);
            ValidateRG(SkillRGMinTBox9, SkillRGMaxTBox9);
            ValidateRG(SkillRGMinTBox10, SkillRGMaxTBox10);
            ValidateRG(SkillRGMinTBox11, SkillRGMaxTBox11);
            ValidateRG(SkillRGMinTBox12, SkillRGMaxTBox12);
            ValidateRG(SkillRGMinTBox13, SkillRGMaxTBox13);
            ValidateRG(SkillRGMinTBox14, SkillRGMaxTBox14);
            ValidateRG(SkillRGMinTBox15, SkillRGMaxTBox15);
            ValidateRG(SkillRGMinTBox16, SkillRGMaxTBox16);
            ValidateRG(SkillRGMinTBox17, SkillRGMaxTBox17);
            ValidateRG(SkillRGMinTBox18, SkillRGMaxTBox18);
            ValidateRG(SkillRGMinTBox19, SkillRGMaxTBox19);
            ValidateRG(SkillRGMinTBox20, SkillRGMaxTBox20);
            ValidateRG(SkillRGMinTBox21, SkillRGMaxTBox21);
            ValidateRG(SkillRGMinTBox22, SkillRGMaxTBox22);

            ValidateRG(GoldRMinTBox1, GoldRMaxTBox1);
        }
        /* random "function" for random girls, 
         * in regular girl randomize it's kinda tolerable to run these directly in button press event, 
         * but since in this case there are two textboxes to fill (and one value is dependant on another) 
         * it's easier to manage if they're done through custom function that I just call in button press event, 
         * and if I need to adjust it I just have one place to adjust
        */
        private void RandomRG(TextBox min, TextBox max, int x, int y, Random rnd)
        {
            int negadj = 0;
            if (x < 0)
            {
                negadj -= x;
                x += negadj;
                y += negadj;
            }
            int xa = rnd.Next(x, y);
            int ya = rnd.Next(xa, y);
            xa -= negadj;
            ya -= negadj;
            min.Text = xa.ToString();
            max.Text = ya.ToString();
        }
        //resets values in input boxes on random girls tab
        private void button_RG_Reset_Click(object sender, EventArgs e)
        {
            nameRTBox1.Text = "";
            descRTBox1.Text = "";
            rgTable.Clear();
            GoldRMinTBox1.Text = "0";
            GoldRMaxTBox1.Text = "10";
            StatRGMinTBox14.Text = "18";
            StatRGMaxTBox14.Text = "18";
            StatRGMinTBox2.Text = StatRGMinTBox12.Text = StatRGMinTBox19.Text = "100";
            StatRGMaxTBox2.Text = StatRGMaxTBox12.Text = StatRGMaxTBox19.Text = "100";
            StatRGMinTBox1.Text = StatRGMinTBox3.Text = StatRGMinTBox4.Text = StatRGMinTBox5.Text = StatRGMinTBox6.Text = StatRGMinTBox7.Text = StatRGMinTBox8.Text = StatRGMinTBox9.Text = StatRGMinTBox10.Text = StatRGMinTBox11.Text = StatRGMinTBox13.Text = StatRGMinTBox15.Text = StatRGMinTBox16.Text = StatRGMinTBox17.Text = StatRGMinTBox18.Text = StatRGMinTBox20.Text = StatRGMinTBox21.Text = StatRGMinTBox22.Text = StatRGMinTBox23.Text = StatRGMinTBox24.Text = StatRGMinTBox25.Text = StatRGMinTBox26.Text = StatRGMinTBox27.Text = "0";
            StatRGMaxTBox1.Text = StatRGMaxTBox3.Text = StatRGMaxTBox4.Text = StatRGMaxTBox5.Text = StatRGMaxTBox6.Text = StatRGMaxTBox7.Text = StatRGMaxTBox8.Text = StatRGMaxTBox9.Text = StatRGMaxTBox10.Text = StatRGMaxTBox11.Text = StatRGMaxTBox13.Text = StatRGMaxTBox15.Text = StatRGMaxTBox16.Text = StatRGMaxTBox17.Text = StatRGMaxTBox18.Text = StatRGMaxTBox20.Text = StatRGMaxTBox21.Text = StatRGMaxTBox22.Text = StatRGMaxTBox23.Text = StatRGMaxTBox24.Text = StatRGMaxTBox25.Text = StatRGMaxTBox26.Text = StatRGMaxTBox27.Text = "0";
            SkillRGMinTBox1.Text = SkillRGMinTBox2.Text = SkillRGMinTBox3.Text = SkillRGMinTBox4.Text = SkillRGMinTBox5.Text = SkillRGMinTBox6.Text = SkillRGMinTBox7.Text = SkillRGMinTBox8.Text = SkillRGMinTBox9.Text = SkillRGMinTBox10.Text = SkillRGMinTBox11.Text = SkillRGMinTBox12.Text = SkillRGMinTBox13.Text = SkillRGMinTBox14.Text = SkillRGMinTBox15.Text = SkillRGMinTBox16.Text = SkillRGMinTBox17.Text = SkillRGMinTBox18.Text = SkillRGMinTBox19.Text = SkillRGMinTBox20.Text = SkillRGMinTBox21.Text = SkillRGMinTBox22.Text = "0";
            SkillRGMaxTBox1.Text = SkillRGMaxTBox2.Text = SkillRGMaxTBox3.Text = SkillRGMaxTBox4.Text = SkillRGMaxTBox5.Text = SkillRGMaxTBox6.Text = SkillRGMaxTBox7.Text = SkillRGMaxTBox8.Text = SkillRGMaxTBox9.Text = SkillRGMaxTBox10.Text = SkillRGMaxTBox11.Text = SkillRGMaxTBox12.Text = SkillRGMaxTBox13.Text = SkillRGMaxTBox14.Text = SkillRGMaxTBox15.Text = SkillRGMaxTBox16.Text = SkillRGMaxTBox17.Text = SkillRGMaxTBox18.Text = SkillRGMaxTBox19.Text = SkillRGMaxTBox20.Text = SkillRGMaxTBox21.Text = SkillRGMaxTBox22.Text = "0";

            this.listBox_RGirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_RGirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
            listBox_RGirlsList.SelectedItem = null;
            this.listBox_RGirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_RGirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
            checkBox_RGHuman.Checked = true;
        }

        //random "function" for normalized values for random girls
        private void RandomRGNorm(TextBox min, TextBox max, int x, int y, Random rnd, int multi)
        {
            int negadj = 0;
            if (x < 0) { negadj -= x; x += negadj; y += negadj; }
            int xa = rnd.Next(x, y); int ya = rnd.Next(xa, y);
            xa -= negadj; ya -= negadj; xa *= multi; ya *= multi;
            min.Text = xa.ToString();
            max.Text = ya.ToString();
        }
        //Randomize stats on random girl tab
        private void buttonRandomizeRG1_Click(object sender, EventArgs e)
        {
            RandomRG(StatRGMinTBox14, StatRGMaxTBox14, 18, 81, rnd);	        //Age
            RandomRG(StatRGMinTBox1, StatRGMaxTBox1, 1, 101, rnd);		        //Charisma
            RandomRG(StatRGMinTBox17, StatRGMaxTBox17, 1, 101, rnd);	        //Beauty  
            RandomRG(StatRGMinTBox24, StatRGMaxTBox24, -100, 101, rnd);	        //Refinement
            RandomRG(StatRGMinTBox8, StatRGMaxTBox8, 1, 101, rnd);		        //Agility
            RandomRG(StatRGMinTBox27, StatRGMaxTBox27, 1, 101, rnd);		    //
            RandomRG(StatRGMinTBox4, StatRGMaxTBox4, 1, 101, rnd);		        //Constitution
            RandomRG(StatRGMinTBox5, StatRGMaxTBox5, 1, 101, rnd);		        //Intelligence
            RandomRG(StatRGMinTBox7, StatRGMaxTBox7, 1, 101, rnd);		        //Mana
            RandomRG(StatRGMinTBox23, StatRGMaxTBox23, -100, 101, rnd);	        //Morality
            RandomRG(StatRGMinTBox25, StatRGMaxTBox25, -100, 101, rnd);	        //Dignity
            RandomRG(StatRGMinTBox6, StatRGMaxTBox6, 1, 101, rnd);		        //Confidence
            RandomRG(StatRGMinTBox15, StatRGMaxTBox15, 1, 101, rnd);	        //Obedience
            RandomRG(StatRGMinTBox16, StatRGMaxTBox16, 1, 101, rnd);	        //Spirit
            RandomRG(StatRGMinTBox3, StatRGMaxTBox3, 1, 101, rnd);		        //Libido
        }
        //Stats normalized randomize, rounded to 10
        private void buttonRGNormRand1_Click(object sender, EventArgs e)
        {
            RandomRG(StatRGMinTBox14, StatRGMaxTBox14, 18, 81, rnd);		    //Age

            RandomRGNorm(StatRGMinTBox1, StatRGMaxTBox1, 1, 11, rnd, 10);		//Charisma
            RandomRGNorm(StatRGMinTBox17, StatRGMaxTBox17, 1, 11, rnd, 10);	    //Beauty  
            RandomRGNorm(StatRGMinTBox24, StatRGMaxTBox24, -10, 11, rnd, 10);	//Refinement
            RandomRGNorm(StatRGMinTBox8, StatRGMaxTBox8, 1, 11, rnd, 10);		//Agility
            RandomRGNorm(StatRGMinTBox27, StatRGMaxTBox27, 1, 11, rnd, 10);		//
            RandomRGNorm(StatRGMinTBox4, StatRGMaxTBox4, 1, 11, rnd, 10);		//Constitution
            RandomRGNorm(StatRGMinTBox5, StatRGMaxTBox5, 1, 11, rnd, 10);		//Intelligence
            RandomRGNorm(StatRGMinTBox7, StatRGMaxTBox7, 1, 11, rnd, 10);		//Mana
            RandomRGNorm(StatRGMinTBox23, StatRGMaxTBox23, -10, 11, rnd, 10);	//Morality
            RandomRGNorm(StatRGMinTBox25, StatRGMaxTBox25, -10, 11, rnd, 10);	//Dignity
            RandomRGNorm(StatRGMinTBox6, StatRGMaxTBox6, 1, 11, rnd, 10);		//Confidence
            RandomRGNorm(StatRGMinTBox15, StatRGMaxTBox15, 1, 11, rnd, 10);	    //Obedience
            RandomRGNorm(StatRGMinTBox16, StatRGMaxTBox16, 1, 11, rnd, 10);	    //Spirit
            RandomRGNorm(StatRGMinTBox3, StatRGMaxTBox3, 1, 11, rnd, 10);		//Libido
        }
        //Stats conditional randomize
        private void buttonRGCondRand1_Click(object sender, EventArgs e)
        {
            RandomRG(StatRGMinTBox14, StatRGMaxTBox14, 18, 71, rnd);		    //Age
            RandomRGNorm(StatRGMinTBox1, StatRGMaxTBox1, 1, 6, rnd, 10);		//Charisma
            RandomRGNorm(StatRGMinTBox17, StatRGMaxTBox17, 3, 8, rnd, 10);	    //Beauty  
            RandomRGNorm(StatRGMinTBox24, StatRGMaxTBox24, -2, 3, rnd, 10);	    //Refinement
            RandomRGNorm(StatRGMinTBox8, StatRGMaxTBox8, 3, 10, rnd, 10);		//Agility
            RandomRGNorm(StatRGMinTBox27, StatRGMaxTBox27, 3, 10, rnd, 10);		//
            RandomRGNorm(StatRGMinTBox4, StatRGMaxTBox4, 3, 6, rnd, 10);		//Constitution
            RandomRGNorm(StatRGMinTBox5, StatRGMaxTBox5, 3, 10, rnd, 10);		//Intelligence
            RandomRGNorm(StatRGMinTBox7, StatRGMaxTBox7, 0, 4, rnd, 10);		//Mana
            RandomRGNorm(StatRGMinTBox23, StatRGMaxTBox23, -2, 3, rnd, 10);	    //Morality
            RandomRGNorm(StatRGMinTBox25, StatRGMaxTBox25, -2, 5, rnd, 10);	    //Dignity
            RandomRGNorm(StatRGMinTBox6, StatRGMaxTBox6, 2, 9, rnd, 10);		//Confidence
            RandomRGNorm(StatRGMinTBox15, StatRGMaxTBox15, 1, 8, rnd, 10);	    //Obedience
            RandomRGNorm(StatRGMinTBox16, StatRGMaxTBox16, 2, 8, rnd, 10);	    //Spirit
            RandomRGNorm(StatRGMinTBox3, StatRGMaxTBox3, 1, 7, rnd, 10);		//Libido
        }

        //Randomize skills on random girl tab
        private void buttonRandomizeRG2_Click(object sender, EventArgs e)
        {
            RandomRG(SkillRGMinTBox2, SkillRGMaxTBox2, 0, 101, rnd);	//Magic Ability
            RandomRG(SkillRGMinTBox8, SkillRGMaxTBox8, 0, 101, rnd);	//Service Skills
            RandomRG(SkillRGMinTBox10, SkillRGMaxTBox10, 0, 101, rnd);	//Combat Ability
            RandomRG(SkillRGMinTBox13, SkillRGMaxTBox13, 0, 101, rnd);	//Medicine
            RandomRG(SkillRGMinTBox14, SkillRGMaxTBox14, 0, 101, rnd);	//Performance
            RandomRG(SkillRGMinTBox16, SkillRGMaxTBox16, 0, 101, rnd);	//Crafting
            RandomRG(SkillRGMinTBox17, SkillRGMaxTBox17, 0, 101, rnd);	//Herbalism
            RandomRG(SkillRGMinTBox18, SkillRGMaxTBox18, 0, 101, rnd);	//Farming
            RandomRG(SkillRGMinTBox19, SkillRGMaxTBox19, 0, 101, rnd);	//Brewing
            RandomRG(SkillRGMinTBox20, SkillRGMaxTBox20, 0, 101, rnd);	//Animal Handling
            RandomRG(SkillRGMinTBox22, SkillRGMaxTBox22, 0, 101, rnd);	//Cooking
        }
        //Skills normalized randomize, rounded to 5
        private void buttonRGNormRand2_Click(object sender, EventArgs e)
        {
            RandomRGNorm(SkillRGMinTBox2, SkillRGMaxTBox2, 0, 21, rnd, 5);		//Magic Ability
            RandomRGNorm(SkillRGMinTBox8, SkillRGMaxTBox8, 0, 21, rnd, 5);		//Service Skills
            RandomRGNorm(SkillRGMinTBox10, SkillRGMaxTBox10, 0, 21, rnd, 5);	//Combat Ability
            RandomRGNorm(SkillRGMinTBox13, SkillRGMaxTBox13, 0, 21, rnd, 5);	//Medicine
            RandomRGNorm(SkillRGMinTBox14, SkillRGMaxTBox14, 0, 21, rnd, 5);	//Performance
            RandomRGNorm(SkillRGMinTBox16, SkillRGMaxTBox16, 0, 21, rnd, 5);	//Crafting
            RandomRGNorm(SkillRGMinTBox17, SkillRGMaxTBox17, 0, 21, rnd, 5);	//Herbalism
            RandomRGNorm(SkillRGMinTBox18, SkillRGMaxTBox18, 0, 21, rnd, 5);	//Farming
            RandomRGNorm(SkillRGMinTBox19, SkillRGMaxTBox19, 0, 21, rnd, 5);	//Brewing
            RandomRGNorm(SkillRGMinTBox20, SkillRGMaxTBox20, 0, 21, rnd, 5);	//Animal Handling
            RandomRGNorm(SkillRGMinTBox22, SkillRGMaxTBox22, 0, 21, rnd, 5);	//Cooking
        }
        //Skills conditional randomize
        private void buttonRGCondRand2_Click(object sender, EventArgs e)
        {
            RandomRGNorm(SkillRGMinTBox2, SkillRGMaxTBox2, 0, 9, rnd, 5);       //Magic
            RandomRGNorm(SkillRGMinTBox10, SkillRGMaxTBox10, 0, 9, rnd, 5);     //Combat
            RandomRGNorm(SkillRGMinTBox8, SkillRGMaxTBox8, 0, 9, rnd, 5);	    //Service Skill
            RandomRGNorm(SkillRGMinTBox13, SkillRGMaxTBox13, 0, 5, rnd, 5);	    //Medicine
            RandomRGNorm(SkillRGMinTBox14, SkillRGMaxTBox14, 0, 9, rnd, 5);	    //Performance
            RandomRGNorm(SkillRGMinTBox16, SkillRGMaxTBox16, 0, 9, rnd, 5);	    //Crafting
            RandomRGNorm(SkillRGMinTBox17, SkillRGMaxTBox17, 0, 5, rnd, 5);	    //Herbalism
            RandomRGNorm(SkillRGMinTBox18, SkillRGMaxTBox18, 0, 5, rnd, 5);	    //Farming
            RandomRGNorm(SkillRGMinTBox19, SkillRGMaxTBox19, 0, 5, rnd, 5);	    //Brewing
            RandomRGNorm(SkillRGMinTBox20, SkillRGMaxTBox20, 0, 9, rnd, 5);	    //Animal Handling
            RandomRGNorm(SkillRGMinTBox22, SkillRGMaxTBox22, 0, 9, rnd, 5);	    //Cooking
        }

        //Randomize sex skills on random girl tab
        private void button_RSS_R_Click(object sender, EventArgs e)
        {
            RandomRG(SkillRGMinTBox1, SkillRGMaxTBox1, 0, 101, rnd);		//Anal Sex
            RandomRG(SkillRGMinTBox3, SkillRGMaxTBox3, 0, 101, rnd);		//BDSM Sex
            RandomRG(SkillRGMinTBox4, SkillRGMaxTBox4, 0, 101, rnd);		//Normal Sex
            RandomRG(SkillRGMinTBox5, SkillRGMaxTBox5, 0, 101, rnd);		//Bestiality Sex
            RandomRG(SkillRGMinTBox6, SkillRGMaxTBox6, 0, 101, rnd);		//Group Sex
            RandomRG(SkillRGMinTBox7, SkillRGMaxTBox7, 0, 101, rnd);		//Lesbian Sex
            RandomRG(SkillRGMinTBox9, SkillRGMaxTBox9, 0, 101, rnd);		//Stripping Sex
            RandomRG(SkillRGMinTBox11, SkillRGMaxTBox11, 0, 101, rnd);		//OralSex
            RandomRG(SkillRGMinTBox12, SkillRGMaxTBox12, 0, 101, rnd);		//TittySex
            RandomRG(SkillRGMinTBox15, SkillRGMaxTBox15, 0, 101, rnd);		//Handjob
            RandomRG(SkillRGMinTBox21, SkillRGMaxTBox21, 0, 101, rnd);		//Footjob
        }
        //Sex Skills normalized randomize, rounded to 5
        private void button_RSS_NR_Click(object sender, EventArgs e)
        {
            RandomRGNorm(SkillRGMinTBox1, SkillRGMaxTBox1, 0, 21, rnd, 5);		//Anal Sex
            RandomRGNorm(SkillRGMinTBox3, SkillRGMaxTBox3, 0, 21, rnd, 5);		//BDSM Sex
            RandomRGNorm(SkillRGMinTBox4, SkillRGMaxTBox4, 0, 21, rnd, 5);		//Normal Sex
            RandomRGNorm(SkillRGMinTBox5, SkillRGMaxTBox5, 0, 21, rnd, 5);		//Bestiality Sex
            RandomRGNorm(SkillRGMinTBox6, SkillRGMaxTBox6, 0, 21, rnd, 5);		//Group Sex
            RandomRGNorm(SkillRGMinTBox7, SkillRGMaxTBox7, 0, 21, rnd, 5);		//Lesbian Sex
            RandomRGNorm(SkillRGMinTBox9, SkillRGMaxTBox9, 0, 21, rnd, 5);		//Stripping Sex
            RandomRGNorm(SkillRGMinTBox11, SkillRGMaxTBox11, 0, 21, rnd, 5);	//OralSex
            RandomRGNorm(SkillRGMinTBox12, SkillRGMaxTBox12, 0, 21, rnd, 5);	//TittySex
            RandomRGNorm(SkillRGMinTBox15, SkillRGMaxTBox15, 0, 21, rnd, 5);	//Handjob
            RandomRGNorm(SkillRGMinTBox21, SkillRGMaxTBox21, 0, 21, rnd, 5);	//Footjob
        }
        //Sex Skills conditional randomize
        private void button_RSS_CR_Click(object sender, EventArgs e)
        {
            RandomRGNorm(SkillRGMinTBox1, SkillRGMaxTBox1, 0, 9, rnd, 5);	    //Anal Sex
            RandomRGNorm(SkillRGMinTBox3, SkillRGMaxTBox3, 0, 9, rnd, 5);	    //BDSM Sex
            RandomRGNorm(SkillRGMinTBox4, SkillRGMaxTBox4, 0, 9, rnd, 5);	    //Normal Sex
            RandomRGNorm(SkillRGMinTBox5, SkillRGMaxTBox5, 0, 9, rnd, 5);	    //Bestiality Sex
            RandomRGNorm(SkillRGMinTBox6, SkillRGMaxTBox6, 0, 9, rnd, 5);	    //Group Sex
            RandomRGNorm(SkillRGMinTBox7, SkillRGMaxTBox7, 0, 9, rnd, 5);	    //Lesbian Sex
            RandomRGNorm(SkillRGMinTBox9, SkillRGMaxTBox9, 0, 9, rnd, 5);	    //Stripping Skill
            RandomRGNorm(SkillRGMinTBox11, SkillRGMaxTBox11, 0, 9, rnd, 5);	    //OralSex
            RandomRGNorm(SkillRGMinTBox12, SkillRGMaxTBox12, 0, 9, rnd, 5);	    //TittySex
            RandomRGNorm(SkillRGMinTBox15, SkillRGMaxTBox15, 0, 9, rnd, 5);	    //Handjob
            RandomRGNorm(SkillRGMinTBox21, SkillRGMaxTBox21, 0, 9, rnd, 5);	    //Footjob
        }

        //button that loads existing random girls file
        private void button_LoadRGirls_Click(object sender, EventArgs e)
        {
            OpenFileDialog Open = new OpenFileDialog();
            Open.Filter = "Whore Master random girls file|*.rgirlsx;*.rgirls|All files|*.*";
            Open.ShowDialog();
            switch (Path.GetExtension(Open.FileName))
            {
                case ".rgirlsx": LoadRGirlsXML(Open.FileName); break;
                case ".rgirls": LoadRGirls(Open.FileName); break;
            }
            SortDataTable(ref RGirlsCollection, ref listBox_RGirlsList);
        }
        //method that parses random girls files to fill listbox and DataTable
        private void LoadRGirls(string path)
        {
            try
            {
                int x = 0;
                StreamReader Import = new StreamReader(path);
                while (Import.Peek() >= 0)
                {
                    x++;
                    string name = Convert.ToString(Import.ReadLine());
                    string desc = Convert.ToString(Import.ReadLine());
                    string data = Convert.ToString(Import.ReadLine()) + "\r\n" + Convert.ToString(Import.ReadLine()) + "\r\n" + Convert.ToString(Import.ReadLine()) + "\r\n" + Convert.ToString(Import.ReadLine());
                    string gold = Convert.ToString(Import.ReadLine()) + "\r\n" + Convert.ToString(Import.ReadLine());
                    string line9 = Convert.ToString(Import.ReadLine());		//is human flag (1 for human, 0 for no human
                    string line10 = Convert.ToString(Import.ReadLine());		//catacombs flag, don't know what it does exactly, manual states it should be 0

                    string num = Convert.ToString(Import.ReadLine());
                    string temp = "";

                    for (int i = 0; i < Convert.ToInt32(num); i++)
                    {
                        if (i + 1 == Convert.ToInt32(num)) temp = temp + Import.ReadLine() + "\r\n" + Import.ReadLine();
                        else temp = temp + Import.ReadLine() + "\r\n" + Import.ReadLine() + "\r\n";
                    }

                    listBox_RGirlsList.Items.Add(name);
                    RGirlsCollection.Rows.Add(name, desc + "\r\n" + data + "\r\n" + gold + "\r\n" + line9 + "\r\n" + line10 + "\r\n" + num + "\r\n" + temp);
                }
                Import.Close();
                StatusLabel1.Text = "Loaded " + x.ToString() + " random girls from file...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("File load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "Open error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "Load canceled...";
            }
        }
        //loads XML random girls file
        private void LoadRGirlsXML(string path)
        {
            try
            {
                XmlTextReader xmlread = new XmlTextReader(path);
                XmlDocument xmldoc = new XmlDocument();
                xmldoc.Load(xmlread);
                XmlNode baseNode = xmldoc.DocumentElement;

                int x = 0;
                foreach (XmlNode node in baseNode.SelectNodes("/Girls/Girl"))
                {
                    x++;
                    string sName = node.Attributes["Name"].Value;				//girl name
                    string sDesc = node.Attributes["Desc"].Value;				//girl description
                    string sHuman = "";											//human or not human flag
                    string sCatacombs = "";
                    switch (node.Attributes["Human"].Value)
                    {
                        case "No":
                        case "no":
                        case "N":
                        case "n":
                        case "0":
                            sHuman = "0"; break;
                        default: sHuman = "1"; break;
                    }
                    switch (node.Attributes["Catacomb"].Value)
                    {
                        case "Yes":
                        case "yes":
                        case "Y":
                        case "y":
                        case "1":
                            sCatacombs = "1"; break;
                        default: sCatacombs = "0"; break;
                    }

                    string sMinMoney = node["Gold"].Attributes["Min"].Value;
                    string sMaxMoney = node["Gold"].Attributes["Max"].Value;


                    string[] aCharisma = new string[2];
                    string[] aHappiness = new string[2];
                    string[] aLibido = new string[2];
                    string[] aConstitution = new string[2];
                    string[] aIntelligence = new string[2];
                    string[] aConfidence = new string[2];
                    string[] aMana = new string[2];
                    string[] aAgility = new string[2];
                    string[] aFame = new string[2];
                    string[] aLevel = new string[2];
                    string[] aAskPrice = new string[2];
                    string[] aHouse = new string[2];
                    string[] aExp = new string[2];
                    string[] aAge = new string[2];
                    string[] aObedience = new string[2];
                    string[] aSpirit = new string[2];
                    string[] aBeauty = new string[2];
                    string[] aTiredness = new string[2];
                    string[] aHealth = new string[2];
                    string[] aPCFear = new string[2];
                    string[] aPCLove = new string[2];
                    string[] aPCHate = new string[2];
                    string[] aMorality = new string[2];
                    string[] aRefinement = new string[2];
                    string[] aDignity = new string[2];
                    string[] aLactation = new string[2];
                    string[] aStrength = new string[2];
                    string[] aNPCLove = new string[2];

                    string[] aAnal = new string[2];
                    string[] aMagic = new string[2];
                    string[] aBDSM = new string[2];
                    string[] aNormalSex = new string[2];
                    string[] aBeastiality = new string[2];
                    string[] aGroup = new string[2];
                    string[] aLesbian = new string[2];
                    string[] aService = new string[2];
                    string[] aStrip = new string[2];
                    string[] aCombat = new string[2];
                    string[] aOralSex = new string[2];
                    string[] aTittySex = new string[2];
                    string[] aMedicine = new string[2];
                    string[] aPerformance = new string[2];
                    string[] aHandjob = new string[2];
                    string[] aCrafting = new string[2];
                    string[] aHerbalism = new string[2];
                    string[] aFarming = new string[2];
                    string[] aBrewing = new string[2];
                    string[] aAnimalHandling = new string[2];
                    string[] aFootjob = new string[2];
                    string[] aCooking = new string[2];

                    //ArrayList alTraits = new ArrayList();
                    //ArrayList alTraitChance = new ArrayList();

                    foreach (XmlNode stat in node.SelectNodes("Stat"))
                    {
                        switch (stat.Attributes["Name"].Value)
                        {
                            case "Charisma":
                                aCharisma[0] = stat.Attributes["Min"].Value;
                                aCharisma[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Happiness":
                                aHappiness[0] = stat.Attributes["Min"].Value;
                                aHappiness[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Libido":
                                aLibido[0] = stat.Attributes["Min"].Value;
                                aLibido[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Constitution":
                                aConstitution[0] = stat.Attributes["Min"].Value;
                                aConstitution[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Intelligence":
                                aIntelligence[0] = stat.Attributes["Min"].Value;
                                aIntelligence[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Confidence":
                                aConfidence[0] = stat.Attributes["Min"].Value;
                                aConfidence[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Mana":
                                aMana[0] = stat.Attributes["Min"].Value;
                                aMana[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Agility":
                                aAgility[0] = stat.Attributes["Min"].Value;
                                aAgility[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Fame":
                                aFame[0] = stat.Attributes["Min"].Value;
                                aFame[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Level":
                                aLevel[0] = stat.Attributes["Min"].Value;
                                aLevel[1] = stat.Attributes["Max"].Value;
                                break;
                            case "AskPrice":
                                aAskPrice[0] = stat.Attributes["Min"].Value;
                                aAskPrice[1] = stat.Attributes["Max"].Value;
                                break;
                            case "House":
                                aHouse[0] = stat.Attributes["Min"].Value;
                                aHouse[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Exp":
                                aExp[0] = stat.Attributes["Min"].Value;
                                aExp[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Age":
                                aAge[0] = stat.Attributes["Min"].Value;
                                aAge[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Obedience":
                                aObedience[0] = stat.Attributes["Min"].Value;
                                aObedience[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Spirit":
                                aSpirit[0] = stat.Attributes["Min"].Value;
                                aSpirit[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Beauty":
                                aBeauty[0] = stat.Attributes["Min"].Value;
                                aBeauty[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Tiredness":
                                aTiredness[0] = stat.Attributes["Min"].Value;
                                aTiredness[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Health":
                                aHealth[0] = stat.Attributes["Min"].Value;
                                aHealth[1] = stat.Attributes["Max"].Value;
                                break;
                            case "PCFear":
                                aPCFear[0] = stat.Attributes["Min"].Value;
                                aPCFear[1] = stat.Attributes["Max"].Value;
                                break;
                            case "PCLove":
                                aPCLove[0] = stat.Attributes["Min"].Value;
                                aPCLove[1] = stat.Attributes["Max"].Value;
                                break;
                            case "PCHate":
                                aPCHate[0] = stat.Attributes["Min"].Value;
                                aPCHate[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Morality":
                                aMorality[0] = stat.Attributes["Min"].Value;
                                aMorality[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Refinement":
                                aRefinement[0] = stat.Attributes["Min"].Value;
                                aRefinement[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Dignity":
                                aDignity[0] = stat.Attributes["Min"].Value;
                                aDignity[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Lactation":
                                aLactation[0] = stat.Attributes["Min"].Value;
                                aLactation[1] = stat.Attributes["Max"].Value;
                                break;
                            case "Strength":
                                aStrength[0] = stat.Attributes["Min"].Value;
                                aStrength[1] = stat.Attributes["Max"].Value;
                                break;
                            case "NPCLove":
                                aNPCLove[0] = stat.Attributes["Min"].Value;
                                aNPCLove[1] = stat.Attributes["Max"].Value;
                                break;
                        }
                    }

                    foreach (XmlNode skill in node.SelectNodes("Skill"))
                    {
                        switch (skill.Attributes["Name"].Value)
                        {
                            case "Anal":
                                aAnal[0] = skill.Attributes["Min"].Value;
                                aAnal[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Magic":
                                aMagic[0] = skill.Attributes["Min"].Value;
                                aMagic[1] = skill.Attributes["Max"].Value;
                                break;
                            case "BDSM":
                                aBDSM[0] = skill.Attributes["Min"].Value;
                                aBDSM[1] = skill.Attributes["Max"].Value;
                                break;
                            case "NormalSex":
                                aNormalSex[0] = skill.Attributes["Min"].Value;
                                aNormalSex[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Beastiality":
                                aBeastiality[0] = skill.Attributes["Min"].Value;
                                aBeastiality[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Group":
                                aGroup[0] = skill.Attributes["Min"].Value;
                                aGroup[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Lesbian":
                                aLesbian[0] = skill.Attributes["Min"].Value;
                                aLesbian[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Service":
                                aService[0] = skill.Attributes["Min"].Value;
                                aService[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Strip":
                                aStrip[0] = skill.Attributes["Min"].Value;
                                aStrip[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Combat":
                                aCombat[0] = skill.Attributes["Min"].Value;
                                aCombat[1] = skill.Attributes["Max"].Value;
                                break;
                            case "OralSex":
                                aOralSex[0] = skill.Attributes["Min"].Value;
                                aOralSex[1] = skill.Attributes["Max"].Value;
                                break;
                            case "TittySex":
                                aTittySex[0] = skill.Attributes["Min"].Value;
                                aTittySex[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Medicine":
                                aMedicine[0] = skill.Attributes["Min"].Value;
                                aMedicine[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Performance":
                                aPerformance[0] = skill.Attributes["Min"].Value;
                                aPerformance[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Handjob":
                                aHandjob[0] = skill.Attributes["Min"].Value;
                                aHandjob[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Crafting":
                                aCrafting[0] = skill.Attributes["Min"].Value;
                                aCrafting[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Herbalism":
                                aHerbalism[0] = skill.Attributes["Min"].Value;
                                aHerbalism[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Farming":
                                aFarming[0] = skill.Attributes["Min"].Value;
                                aFarming[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Brewing":
                                aBrewing[0] = skill.Attributes["Min"].Value;
                                aBrewing[1] = skill.Attributes["Max"].Value;
                                break;
                            case "AnimalHandling":
                                aAnimalHandling[0] = skill.Attributes["Min"].Value;
                                aAnimalHandling[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Footjob":
                                aFootjob[0] = skill.Attributes["Min"].Value;
                                aFootjob[1] = skill.Attributes["Max"].Value;
                                break;
                            case "Cooking":
                                aCooking[0] = skill.Attributes["Min"].Value;
                                aCooking[1] = skill.Attributes["Max"].Value;
                                break;
                                
                        }
                    }


                    int tnum = node.SelectNodes("Trait").Count;
                    int tcount = 0;
                    string[,] arTraits = new string[tnum, 2];

                    foreach (XmlNode trait in node.SelectNodes("Trait"))
                    {
                        arTraits[tcount, 0] = trait.Attributes["Name"].Value;
                        arTraits[tcount, 1] = trait.Attributes["Percent"].Value;
                        tcount++;
                    }

                    string sMinStat = aCharisma[0] + " " + aHappiness[0] + " " + aLibido[0] + " " + aConstitution[0] + " " + aIntelligence[0] + " " + aConfidence[0] + " " + aMana[0] + " " + aAgility[0] + " " + aFame[0] + " " + aLevel[0] + " " + aAskPrice[0] + " " + aHouse[0] + " " + aExp[0] + " " + aAge[0] + " " + aObedience[0] + " " + aSpirit[0] + " " + aBeauty[0] + " " + aTiredness[0] + " " + aHealth[0] + " " + aPCFear[0] + " " + aPCLove[0] + " " + aPCHate[0] + " " + aMorality[0] + " " + aRefinement[0] + " " + aDignity[0] + " " + aLactation[0] + " " + aStrength[0] + " " + aNPCLove[0];
                    string sMaxStat = aCharisma[1] + " " + aHappiness[1] + " " + aLibido[1] + " " + aConstitution[1] + " " + aIntelligence[1] + " " + aConfidence[1] + " " + aMana[1] + " " + aAgility[1] + " " + aFame[1] + " " + aLevel[1] + " " + aAskPrice[1] + " " + aHouse[1] + " " + aExp[1] + " " + aAge[1] + " " + aObedience[1] + " " + aSpirit[1] + " " + aBeauty[1] + " " + aTiredness[1] + " " + aHealth[1] + " " + aPCFear[1] + " " + aPCLove[1] + " " + aPCHate[1] + " " + aMorality[1] + " " + aRefinement[1] + " " + aDignity[1] + " " + aLactation[1] + " " + aStrength[1] + " " + aNPCLove[1];

                    string sMinSkill = aAnal[0] + " " + aMagic[0] + " " + aBDSM[0] + " " + aNormalSex[0] + " " + aBeastiality[0] + " " + aGroup[0] + " " + aLesbian[0] + " " + aService[0] + " " + aStrip[0] + " " + aCombat[0] + " " + aOralSex[0] + " " + aTittySex[0] + " " + aMedicine[0] + " " + aPerformance[0] + " " + aHandjob[0] + " " + aCrafting[0] + " " + aHerbalism[0] + " " + aFarming[0] + " " + aBrewing[0] + " " + aAnimalHandling[0] + " " + aFootjob[0] + " " + aCooking[0];
                    string sMaxSkill = aAnal[1] + " " + aMagic[1] + " " + aBDSM[1] + " " + aNormalSex[1] + " " + aBeastiality[1] + " " + aGroup[1] + " " + aLesbian[1] + " " + aService[1] + " " + aStrip[1] + " " + aCombat[1] + " " + aOralSex[1] + " " + aTittySex[1] + " " + aMedicine[1] + " " + aPerformance[1] + " " + aHandjob[1] + " " + aCrafting[1] + " " + aHerbalism[1] + " " + aFarming[1] + " " + aBrewing[1] + " " + aAnimalHandling[1] + " " + aFootjob[1] + " " + aCooking[1];

                    string sTraits = tnum.ToString();
                    for (int y = 0; y < tnum; y++)
                    {
                        sTraits = sTraits + "\r\n" + arTraits[y, 0] + "\r\n" + arTraits[y, 1];
                    }

                    RGirlsCollection.Rows.Add(sName, sDesc + "\r\n" + sMinStat + "\r\n" + sMaxStat + "\r\n" + sMinSkill + "\r\n" + sMaxSkill + "\r\n" + sMinMoney + "\r\n" + sMaxMoney + "\r\n" + sHuman + "\r\n" + sCatacombs + "\r\n" + sTraits);
                    listBox_RGirlsList.Items.Add(sName);
                }
                xmlread.Close();
                StatusLabel1.Text = "Loaded " + x.ToString() + " girls from XML file...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("XML load error error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "XML load error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "XML load error...";
            }
        }
        //button that deletes selected random girl from list
        private void button_DeleteRGirl_Click(object sender, EventArgs e)
        {
            try
            {
                this.listBox_RGirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_RGirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
                StatusLabel1.Text = "\"" + listBox_RGirlsList.SelectedItem + "\"" + " deleted...";
                RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex].Delete();
                listBox_RGirlsList.Items.RemoveAt(listBox_RGirlsList.SelectedIndex);
                this.listBox_RGirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_RGirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(listBox_RGirlsList.SelectedIndex, err.Message);
                MessageBox.Show("It's probably nothing serious, i.e. you pressed Delete button\n\rwhile list was empty or nothing was selected, now for \"official\" error:\n\r\n\r" + err.Message, "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "Delete item error...";
            }
        }
        //button that compiles random girls data back from dataTable to output tab
        private void button_SaveRGirls_Click(object sender, EventArgs e)
        {
            Filesave.FileName = "";
            Filesave.Filter = "Whore Master random girls XML file|*.rgirlsx|All files|*.*";
            Filesave.DefaultExt = ".rgirlsx";
            Filesave.ShowDialog();
            try
            {
                if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
                if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");
                SaveRGirlsXML(Filesave.FileName);
                StatusLabel1.Text = "Successfully compiled " + listBox_RGirlsList.Items.Count.ToString() + " random girls...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(Filesave.FileName, err.Message);
                StatusLabel1.Text = "Save Error.";
            }
        }
        //save girls in xml file function
        private void SaveRGirlsXML(string path)
        {
            XmlDocument xmldoc = new XmlDocument();
            
            XmlElement girls = xmldoc.CreateElement("Girls");
            XmlElement girl = xmldoc.CreateElement("Girl");
            XmlElement cash = xmldoc.CreateElement("Cash");
            XmlElement stat = xmldoc.CreateElement("Stat");
            XmlElement skill = xmldoc.CreateElement("Skill");
            XmlElement trait = xmldoc.CreateElement("Trait");

            string[] sStats = new string[28] { "Charisma", "Happiness", "Libido", "Constitution", "Intelligence", "Confidence", "Mana", "Agility", "Fame", "Level", "AskPrice", "House", "Exp", "Age", "Obedience", "Spirit", "Beauty", "Tiredness", "Health", "PCFear", "PCLove", "PCHate", "Morality", "Refinement", "Dignity", "Lactation", "Strength", "NPCLove" };
            string[] sSkills = new string[22] { "Anal", "Magic", "BDSM", "NormalSex", "Beastiality", "Group", "Lesbian", "Service", "Strip", "Combat", "OralSex", "TittySex", "Medicine", "Performance", "Handjob", "Crafting", "Herbalism", "Farming", "Brewing", "AnimalHandling", "Footjob", "Cooking" };
            int[] statsortorder = new int[28] { 9, 12, 13, 8, 10, 11, 18, 1, 17, 20, 19, 21, 25, 0, 16, 23, 7, 26, 3, 4, 6, 22, 24, 5, 14, 15, 2, 27 };
            int[] skillsortorder = new int[22] { 9, 1, 7, 12, 13, 15, 17, 16, 18, 19, 21, 3, 0, 2, 4, 6, 8, 5, 10, 11, 14, 20 };

            xmldoc.AppendChild(girls);

            for (int x = 0; x < RGirlsCollection.Rows.Count; x++)
            {
                girl = xmldoc.CreateElement("Girl");

                StringReader sData = new StringReader(RGirlsCollection.Rows[x][1].ToString());

                string sName = RGirlsCollection.Rows[x][0].ToString();
                string sDesc = sData.ReadLine();

                string[] sMinStat = sData.ReadLine().Split(' ');
                string[] sMaxStat = sData.ReadLine().Split(' ');
                string[] sMinSkill = sData.ReadLine().Split(' ');
                string[] sMaxSkill = sData.ReadLine().Split(' ');

                string sMinMoney = sData.ReadLine();
                string sMaxMoney = sData.ReadLine();

                string sHuman = "";

                switch (sData.ReadLine())
                {
                    case "0": sHuman = "No"; break;
                    default: sHuman = "Yes"; break;
                }
                girl.SetAttribute("Name", sName);
                girl.SetAttribute("Desc", sDesc);
                girl.SetAttribute("Human", sHuman);

                cash = xmldoc.CreateElement("Gold");
                cash.SetAttribute("Min", sMinMoney);
                cash.SetAttribute("Max", sMaxMoney);
                girl.AppendChild(cash);
                for (int y = 0; y < sStats.Count(); y++)
                {
                    stat = xmldoc.CreateElement("Stat");
                    stat.SetAttribute("Name", sStats[statsortorder[y]]);
                    if (y >= sMinStat.Length)
                    {
                        stat.SetAttribute("Min", "0");
                        stat.SetAttribute("Max", "0");
                    }
                    else
                    {
                        stat.SetAttribute("Min", sMinStat[statsortorder[y]]);
                        stat.SetAttribute("Max", sMaxStat[statsortorder[y]]);
                    }
                    girl.AppendChild(stat);
                }
                for (int y = 0; y < sSkills.Count(); y++)
                {
                    skill = xmldoc.CreateElement("Skill");
                    skill.SetAttribute("Name", sSkills[skillsortorder[y]]);
                    skill.SetAttribute("Min", sMinSkill[skillsortorder[y]]);
                    skill.SetAttribute("Max", sMaxSkill[skillsortorder[y]]);
                    girl.AppendChild(skill);
                }
                string sCatacombs = "";
                switch (sData.ReadLine())
                {
                    case "0": sCatacombs = "No"; break;
                    case "1": sCatacombs = "Yes"; break;
                }
                girl.SetAttribute("Catacomb", sCatacombs);
                int iTraitNum = Convert.ToInt32(sData.ReadLine());
                for (int y = 0; y < iTraitNum; y++)
                {
                    trait = xmldoc.CreateElement("Trait");
                    trait.SetAttribute("Name", sData.ReadLine());
                    trait.SetAttribute("Percent", sData.ReadLine());
                    girl.AppendChild(trait);
                }
                girls.AppendChild(girl);
            }

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.NewLineOnAttributes = false;
            settings.IndentChars = "    ";
            XmlWriter xmlwrite = XmlWriter.Create(path, settings);

            xmldoc.Save(xmlwrite);
            xmlwrite.Close();
        }

        //clears random girls list
        private void button_RGirlsClearList_Click(object sender, EventArgs e)
        {
            RGirlsCollection.Clear();
            listBox_RGirlsList.Items.Clear();
            toolTip1.SetToolTip(listBox_RGirlsList, "");
            StatusLabel1.Text = "Random girl list cleared...";
        }
        //parses selected random girl to enable editing
        private void listBox_RGirlsList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox_RGirlsList.SelectedIndex < 0) return;
            nameRTBox1.Text = RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][0].ToString();
            string sData = RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][1].ToString();

            StringReader data = new StringReader(sData);
            descRTBox1.Text = data.ReadLine();


            char[] separator = { ' ' };											//data in item data is space delimited, so to begin extraction from it we create an array of separator characters
            string[] values = data.ReadLine().Split(separator);					//use Split function to put every value that's "between" spaces to array
            StatRGMinTBox1.Text = values[0]; StatRGMinTBox2.Text = values[1]; StatRGMinTBox3.Text = values[2]; StatRGMinTBox4.Text = values[3]; StatRGMinTBox5.Text = values[4]; StatRGMinTBox6.Text = values[5]; StatRGMinTBox7.Text = values[6]; StatRGMinTBox8.Text = values[7]; StatRGMinTBox9.Text = values[8]; StatRGMinTBox10.Text = values[9]; StatRGMinTBox11.Text = values[10]; StatRGMinTBox12.Text = values[11]; StatRGMinTBox14.Text = values[13]; StatRGMinTBox15.Text = values[14]; StatRGMinTBox16.Text = values[15]; StatRGMinTBox17.Text = values[16]; StatRGMinTBox18.Text = values[17]; StatRGMinTBox19.Text = values[18]; StatRGMinTBox20.Text = values[19]; StatRGMinTBox21.Text = values[20];
            StatRGMinTBox22.Text = values[21];
            StatRGMinTBox23.Text = (values.Length > 22) ? values[22] : "0";
            StatRGMinTBox24.Text = (values.Length > 23) ? values[23] : "0";
            StatRGMinTBox25.Text = (values.Length > 24) ? values[24] : "0";
            StatRGMinTBox26.Text = (values.Length > 25) ? values[25] : "0";
            StatRGMinTBox27.Text = (values.Length > 26) ? values[26] : "0";
            //StatRGMinTBox28.Text = (values.Length > 27) ? values[27] : "0";     // `J` Crazy added this but it is not going to be editable yet

            values = data.ReadLine().Split(separator);
            StatRGMaxTBox1.Text = values[0]; StatRGMaxTBox2.Text = values[1]; StatRGMaxTBox3.Text = values[2]; StatRGMaxTBox4.Text = values[3]; StatRGMaxTBox5.Text = values[4]; StatRGMaxTBox6.Text = values[5]; StatRGMaxTBox7.Text = values[6]; StatRGMaxTBox8.Text = values[7]; StatRGMaxTBox9.Text = values[8]; StatRGMaxTBox10.Text = values[9]; StatRGMaxTBox11.Text = values[10]; StatRGMaxTBox12.Text = values[11]; StatRGMaxTBox14.Text = values[13]; StatRGMaxTBox15.Text = values[14]; StatRGMaxTBox16.Text = values[15]; StatRGMaxTBox17.Text = values[16]; StatRGMaxTBox18.Text = values[17]; StatRGMaxTBox19.Text = values[18]; StatRGMaxTBox20.Text = values[19]; StatRGMaxTBox21.Text = values[20];
            StatRGMaxTBox22.Text = values[21];
            StatRGMaxTBox23.Text = (values.Length > 22) ? values[22] : "0";
            StatRGMaxTBox24.Text = (values.Length > 23) ? values[23] : "0";
            StatRGMaxTBox25.Text = (values.Length > 24) ? values[24] : "0";
            StatRGMaxTBox26.Text = (values.Length > 25) ? values[25] : "0";
            StatRGMaxTBox27.Text = (values.Length > 26) ? values[26] : "0";
            //StatRGMaxTBox28.Text = (values.Length > 27) ? values[27] : "0";     // `J` Crazy added this but it is not going to be editable yet


            values = data.ReadLine().Split(separator);
            SkillRGMinTBox1.Text = (values.Length > 00) ? values[0] : "0";
            SkillRGMinTBox2.Text = (values.Length > 01) ? values[1] : "0";
            SkillRGMinTBox3.Text = (values.Length > 02) ? values[2] : "0";
            SkillRGMinTBox4.Text = (values.Length > 03) ? values[3] : "0";
            SkillRGMinTBox5.Text = (values.Length > 04) ? values[4] : "0";
            SkillRGMinTBox6.Text = (values.Length > 05) ? values[5] : "0";
            SkillRGMinTBox7.Text = (values.Length > 06) ? values[6] : "0";
            SkillRGMinTBox8.Text = (values.Length > 07) ? values[7] : "0";
            SkillRGMinTBox9.Text = (values.Length > 08) ? values[8] : "0";
            SkillRGMinTBox10.Text = (values.Length > 09) ? values[9] : "0";
            SkillRGMinTBox11.Text = (values.Length > 10) ? values[10] : "0";
            SkillRGMinTBox12.Text = (values.Length > 11) ? values[11] : "0";
            SkillRGMinTBox13.Text = (values.Length > 12) ? values[12] : "0";
            SkillRGMinTBox14.Text = (values.Length > 13) ? values[13] : "0";
            SkillRGMinTBox15.Text = (values.Length > 14) ? values[14] : "0";
            SkillRGMinTBox16.Text = (values.Length > 15) ? values[15] : "0";
            SkillRGMinTBox17.Text = (values.Length > 16) ? values[16] : "0";
            SkillRGMinTBox18.Text = (values.Length > 17) ? values[17] : "0";
            SkillRGMinTBox19.Text = (values.Length > 18) ? values[18] : "0";
            SkillRGMinTBox20.Text = (values.Length > 19) ? values[19] : "0";
            SkillRGMinTBox21.Text = (values.Length > 20) ? values[20] : "0";
            SkillRGMinTBox22.Text = (values.Length > 21) ? values[21] : "0";

            values = data.ReadLine().Split(separator);
            SkillRGMaxTBox1.Text = (values.Length > 00) ? values[0] : "0";
            SkillRGMaxTBox2.Text = (values.Length > 01) ? values[1] : "0";
            SkillRGMaxTBox3.Text = (values.Length > 02) ? values[2] : "0";
            SkillRGMaxTBox4.Text = (values.Length > 03) ? values[3] : "0";
            SkillRGMaxTBox5.Text = (values.Length > 04) ? values[4] : "0";
            SkillRGMaxTBox6.Text = (values.Length > 05) ? values[5] : "0";
            SkillRGMaxTBox7.Text = (values.Length > 06) ? values[6] : "0";
            SkillRGMaxTBox8.Text = (values.Length > 07) ? values[7] : "0";
            SkillRGMaxTBox9.Text = (values.Length > 08) ? values[8] : "0";
            SkillRGMaxTBox10.Text = (values.Length > 09) ? values[9] : "0";
            SkillRGMaxTBox11.Text = (values.Length > 10) ? values[10] : "0";
            SkillRGMaxTBox12.Text = (values.Length > 11) ? values[11] : "0";
            SkillRGMaxTBox13.Text = (values.Length > 12) ? values[12] : "0";
            SkillRGMaxTBox14.Text = (values.Length > 13) ? values[13] : "0";
            SkillRGMaxTBox15.Text = (values.Length > 14) ? values[14] : "0";
            SkillRGMaxTBox16.Text = (values.Length > 15) ? values[15] : "0";
            SkillRGMaxTBox17.Text = (values.Length > 16) ? values[16] : "0";
            SkillRGMaxTBox18.Text = (values.Length > 17) ? values[17] : "0";
            SkillRGMaxTBox19.Text = (values.Length > 18) ? values[18] : "0";
            SkillRGMaxTBox20.Text = (values.Length > 19) ? values[19] : "0";
            SkillRGMaxTBox21.Text = (values.Length > 20) ? values[20] : "0";
            SkillRGMaxTBox22.Text = (values.Length > 21) ? values[21] : "0";

            GoldRMinTBox1.Text = data.ReadLine();
            GoldRMaxTBox1.Text = data.ReadLine();

            string line9 = data.ReadLine();     // "is human" flag
            switch (line9)
            {
                case "0": checkBox_RGHuman.Checked = false; break;
                default: checkBox_RGHuman.Checked = true; break;
            }
            string line10 = data.ReadLine();    // "is catacomb" flag
            switch (line10)
            {
                case "1": checkBox_RGCatacombs.Checked = true; break;
                default: checkBox_RGCatacombs.Checked = false; break;
            }

            int traitNum = Convert.ToInt32(data.ReadLine());
            rgTable.Clear();
            for (int i = 0; i < traitNum; i++)
            {
                rgTable.Rows.Add(data.ReadLine(), data.ReadLine());
            }
            StatusLabel1.Text = "\"" + nameRTBox1.Text + "\"" + " parsed successfully...";
        }

        //lock unlock value field in random girls trait list
        private void checkBox_RGTraitsReadOnly_CheckedChanged(object sender, EventArgs e)
        {
            dataGridView1.Columns[1].ReadOnly = !(dataGridView1.Columns[1].ReadOnly);
        }

        //displays tooltips with random girl descriptions on rgirls list
        private void listBox_RGirlsList_MouseMove(object sender, MouseEventArgs e)
        {
            ListBox listBox = (ListBox)sender;								//creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
            int index = listBox.IndexFromPoint(e.Location);					//e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
            if (index > -1 && index < listBox.Items.Count)					//if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
            {
                string tip = listBox.Items[index].ToString();				//this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
                if (tip != lastTip)											//every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
                {															//also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
                    toolTip1.SetToolTip(listBox, new StringReader(RGirlsCollection.Rows[index][1].ToString()).ReadLine());	//this is the only difference from previous tooltip function. Description is stored in dataTable at [index][1], along with other data, so to get it out we first need to identify coordinates (that's what [index][1] is for), then construct StringReader from that long string, and lastly read first line from that stream, this is description. Luckily it can be fitted in one line :P
                    lastTip = tip;											//updates lastTip string
                }
            }
        }



        private void button_SaveIndividualGirls_Click(object sender, EventArgs e)
        {

            string create = DateTime.Now.GetHashCode().ToString();
            Directory.CreateDirectory(".\\" + create + "\\");
            try
            {
                SaveIndividualGirlsXML(".\\" + create + "\\");
                SaveIndividualRGirlsXML(".\\" + create + "\\");
                SaveIndividualItemsXML(".\\" + create + "\\");

                StatusLabel1.Text = "Successfully compiled " + 
                    listBox_GirlsList.Items.Count.ToString() + " girls, " +
                    listBox_RGirlsList.Items.Count.ToString() + " rgirls and " +
                    listBox_ItemsList.Items.Count.ToString() + " items to folder " + create;
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(Filesave.FileName, err.Message);
                StatusLabel1.Text = "Save Error.";
            }
        }
        private void SaveIndividualGirlsXML(string path)
        {																//and this is other part of the wrapper, this one reads old format from memory and stores it in XML
            for (int x = 0; x < GirlsCollection.Rows.Count; x++)			//and now we need to go through each girl, i.e. row in DataTable
            {
                string filepath = path;

                XmlDocument xmldoc = new XmlDocument();						//first we'll create XmlDocument
                XmlElement girls = xmldoc.CreateElement("Girls");			//and create nodes that are used in this document
                XmlElement girl = xmldoc.CreateElement("Girl");
                XmlElement trait = xmldoc.CreateElement("Trait");

                xmldoc.AppendChild(girls);									//"Girls" is the root node so we append it to our xmldoc first


                girl = xmldoc.CreateElement("Girl");						//I think that creation of child nodes could be placed here instead of there, but this way code looks more readable, anyway, here's where we create our "Girl" node for each girl

                StringReader sData = new StringReader(GirlsCollection.Rows[x][1].ToString());	//girl data is stored in string that's stored in second column of each row in DataTable, to be able to read it easier we'll access that through StringReader object, it has similar properties as TextReader

                string sName = GirlsCollection.Rows[x][0].ToString();	//girl name is stored in first column of the row so we can get to it directly
                string sDesc = sData.ReadLine();							//girl description is first line of description

                int num = Convert.ToInt32(sData.ReadLine());				//after description we get to traits, first line after description is number of traits
                bool foundvirgintrait = false;
                for (int y = 0; y < num; y++)							//we'll use it to know how many lines need to be read (i.e. how many lines are traits)
                {
                    trait = xmldoc.CreateElement("Trait");				//each trait is stored in separate child node under "Girl" node, so here's where that's done
                    string traitname = sData.ReadLine();
                    trait.SetAttribute("Name", traitname);		//Trait name is attribute in "Trait" node, this is where that attribute get's set
                    girl.AppendChild(trait);								//and after that's done append that child node to "Girl" node, rinse and repeat for every trait
                    if (traitname == "Virgin") foundvirgintrait = true;
                }

                string sZero = sData.ReadLine();				//This line is always 0, it's not used for now, but if need be it's stored anyway, it's just not currently saved to XML (there are no attribute for it)
                string[] sStats = sData.ReadLine().Split(' ');	//line with stats, stored in string array, it's basically space delimited data so it get's splitted right away
                string[] sSkills = sData.ReadLine().Split(' ');	//and again for skills
                string sGold = sData.ReadLine();				//gold
                string sVirgin = sData.ReadLine();				//virgin
                string sStatus = sData.ReadLine();				//status, slave, normal, catacombs, arena, your daughter
                string sAccomm = sData.ReadLine();				//Accommodation
                if (!foundvirgintrait && sVirgin == "1")          // add virgin trait if not already there and Virgin box is checked.
                {
                    trait.SetAttribute("Name", "Virgin");		//Trait name is attribute in "Trait" node, this is where that attribute get's set
                    girl.AppendChild(trait);								//and after that's done append that child node to "Girl" node, rinse and repeat for every trait
                }
                switch (sStatus)											//and here's where it gets "translated" to old XML, on old format it's represented with number, in new with string, so this simple replaces that number with string
                {
                    case "1": sStatus = "Slave"; break;
                    case "2": sStatus = "Catacombs"; break;
                    case "3": sStatus = "Arena"; break;
                    case "4": sStatus = "Your Daughter"; break;
                    case "0":
                    default: sStatus = "Normal"; break;
                }

                string sFName = sData.ReadLine(); if (sFName == null) sFName = "";
                string sMName = sData.ReadLine(); if (sMName == null) sMName = "";
                string sLName = sData.ReadLine(); if (sLName == null) sLName = "";

                if (sName.Length < 1 && (sFName.Length > 1 || sMName.Length > 1 || sLName.Length < 1))
                {
                    sName = G_Create_Real_Name(sFName, sMName, sLName);
                }
                else if (sName.Length > 0 && sFName.Length < 1 && sMName.Length < 1 && sLName.Length < 1)
                {
                    string[] names = new string[3];
                    names = G_Divide_Real_Name(sName);
                    sFName= names[0];
                    sMName= names[1];
                    sLName = names[2];

                }

                //now that we have all that data isolated it should be assigned to their respective attributes in "Girl" node, this is what's done here, bunch of attributes getting assingned their values
                girl.SetAttribute("Name", sName);
                girl.SetAttribute("FirstName", sFName);
                girl.SetAttribute("MiddleName", sMName);
                girl.SetAttribute("Surname", sLName);
                girl.SetAttribute("Desc", sDesc);

                girl.SetAttribute("Level", sStats[9]);
                girl.SetAttribute("Exp", sStats[12]);
                girl.SetAttribute("Age", sStats[13]);
                girl.SetAttribute("Fame", sStats[8]);
                girl.SetAttribute("AskPrice", sStats[10]);
                girl.SetAttribute("House", sStats[11]);
                girl.SetAttribute("Accomm", sAccomm);
                girl.SetAttribute("Health", sStats[18]);
                girl.SetAttribute("Happiness", sStats[1]);
                girl.SetAttribute("Tiredness", sStats[17]);
                girl.SetAttribute("PCLove", sStats[20]);
                girl.SetAttribute("PCFear", sStats[19]);
                girl.SetAttribute("PCHate", sStats[21]);
                girl.SetAttribute("Lactation", sStats[25]);
                girl.SetAttribute("Strength", sStats[26]);
                girl.SetAttribute("NPCLove", sStats[27]);

                girl.SetAttribute("Charisma", sStats[0]);
                girl.SetAttribute("Beauty", sStats[16]);
                girl.SetAttribute("Refinement", sStats[23]);
                girl.SetAttribute("Agility", sStats[7]);
                girl.SetAttribute("Constitution", sStats[3]);
                girl.SetAttribute("Intelligence", sStats[4]);
                girl.SetAttribute("Mana", sStats[6]);
                girl.SetAttribute("Morality", sStats[22]);
                girl.SetAttribute("Dignity", sStats[24]);
                girl.SetAttribute("Confidence", sStats[5]);
                girl.SetAttribute("Obedience", sStats[14]);
                girl.SetAttribute("Spirit", sStats[15]);
                girl.SetAttribute("Libido", sStats[2]);

                girl.SetAttribute("Combat", sSkills[9]);
                girl.SetAttribute("Magic", sSkills[1]);
                girl.SetAttribute("Service", sSkills[7]);
                girl.SetAttribute("Medicine", sSkills[12]);
                girl.SetAttribute("Performance", sSkills[13]);
                girl.SetAttribute("Crafting", sSkills[15]);
                girl.SetAttribute("Farming", sSkills[17]);
                girl.SetAttribute("Herbalism", sSkills[16]);
                girl.SetAttribute("Brewing", sSkills[18]);
                girl.SetAttribute("AnimalHandling", sSkills[19]);
                girl.SetAttribute("Cooking", sSkills[21]);

                girl.SetAttribute("NormalSex", sSkills[3]);
                girl.SetAttribute("Anal", sSkills[0]);
                girl.SetAttribute("BDSM", sSkills[2]);
                girl.SetAttribute("Beastiality", sSkills[4]);
                girl.SetAttribute("Lesbian", sSkills[6]);
                girl.SetAttribute("Strip", sSkills[8]);
                girl.SetAttribute("Group", sSkills[5]);
                girl.SetAttribute("OralSex", sSkills[10]);
                girl.SetAttribute("TittySex", sSkills[11]);
                girl.SetAttribute("Handjob", sSkills[14]);
                girl.SetAttribute("Footjob", sSkills[20]);

                girl.SetAttribute("Gold", sGold);
                girl.SetAttribute("Virgin", sVirgin);
                girl.SetAttribute("Status", sStatus);

                girls.AppendChild(girl);								//finaly done (with one girl), all her data has been read and assigned to nodes or attributes, now we only need to "stick" this finished "Girl" child node to root "Girls" node and this is what this does. and then goes at begining of the loop for the next one



                int test = 0;
                string filename = filepath + sName + ".girlsx";

                while (File.Exists(Convert.ToString(filename)) == true)
                {
                    test++;
                    if (!Directory.Exists(filepath + test.ToString())) Directory.CreateDirectory(filepath + test.ToString());
                    filename = filepath + test.ToString() + "\\" + sName + ".girlsx";
                }


                //after it's all done we have our XML, although, only in memory, not stored somewhere safe
                XmlWriterSettings settings = new XmlWriterSettings();	//I've tried few ways of saving this, and this had the nicest output (they were all correct XML wise mind you, but output of this has the "nicest" structure as far as human readability goes
                settings.Indent = true;									//indent every node, otherwise it would be harder to find where each node begins, again, not for computer, for some person looking at outputed XML
                settings.NewLineOnAttributes = true;						//without this each node would be one long line, this puts each attribute in new line
                settings.IndentChars = "    ";								//just a character that'll be used for indenting, \t means tab, so indent is one tab,
                XmlWriter xmlwrite = XmlWriter.Create(filename, settings);	//now that settings are complete we can write this file, using path passed from button function, and settings we just made

                xmldoc.Save(xmlwrite);									//now we tell our XmlDocument to save itself to our XmlWriter, this is what finally gives us our file
                xmlwrite.Close();										//now to be all nice and proper we close our file, after all it's finished


            }
        }
        private void SaveIndividualRGirlsXML(string path)
        {

            for (int x = 0; x < RGirlsCollection.Rows.Count; x++)
            {
                string filepath = path;
                XmlDocument xmldoc = new XmlDocument();

                XmlElement girls = xmldoc.CreateElement("Girls");
                XmlElement girl = xmldoc.CreateElement("Girl");
                XmlElement cash = xmldoc.CreateElement("Cash");
                XmlElement stat = xmldoc.CreateElement("Stat");
                XmlElement skill = xmldoc.CreateElement("Skill");
                XmlElement trait = xmldoc.CreateElement("Trait");

                string[] sStats = new string[28] { "Charisma", "Happiness", "Libido", "Constitution", "Intelligence", "Confidence", "Mana", "Agility", "Fame", "Level", "AskPrice", "House", "Exp", "Age", "Obedience", "Spirit", "Beauty", "Tiredness", "Health", "PCFear", "PCLove", "PCHate", "Morality", "Refinement", "Dignity", "Lactation", "Strength", "NPCLove" };
                string[] sSkills = new string[22] { "Anal", "Magic", "BDSM", "NormalSex", "Beastiality", "Group", "Lesbian", "Service", "Strip", "Combat", "OralSex", "TittySex", "Medicine", "Performance", "Handjob", "Crafting", "Herbalism", "Farming", "Brewing", "AnimalHandling", "Footjob", "Cooking" };
                int[] statsortorder = new int[28] { 9, 12, 13, 8, 10, 11, 18, 1, 17, 20, 19, 21, 25, 0, 16, 23, 7, 26, 3, 4, 6, 22, 24, 5, 14, 15, 2, 27 };
                int[] skillsortorder = new int[22] { 9, 1, 7, 12, 13, 15, 17, 16, 18, 19, 21, 3, 0, 2, 4, 6, 8, 5, 10, 11, 14, 20};

                xmldoc.AppendChild(girls);

                girl = xmldoc.CreateElement("Girl");

                StringReader sData = new StringReader(RGirlsCollection.Rows[x][1].ToString());

                string sName = RGirlsCollection.Rows[x][0].ToString();
                string sDesc = sData.ReadLine();

                string[] sMinStat = sData.ReadLine().Split(' ');
                string[] sMaxStat = sData.ReadLine().Split(' ');
                string[] sMinSkill = sData.ReadLine().Split(' ');
                string[] sMaxSkill = sData.ReadLine().Split(' ');

                string sMinMoney = sData.ReadLine();
                string sMaxMoney = sData.ReadLine();

                string sHuman = "";

                switch (sData.ReadLine())
                {
                    case "0": sHuman = "No"; break;
                    default: sHuman = "Yes"; break;
                }
                girl.SetAttribute("Name", sName);
                girl.SetAttribute("Desc", sDesc);
                girl.SetAttribute("Human", sHuman);

                cash = xmldoc.CreateElement("Gold");
                cash.SetAttribute("Min", sMinMoney);
                cash.SetAttribute("Max", sMaxMoney);
                girl.AppendChild(cash);
                for (int y = 0; y < sStats.Count(); y++)
                {
                    stat = xmldoc.CreateElement("Stat");
                    stat.SetAttribute("Name", sStats[statsortorder[y]]);
                    if (y < sMinStat.Length)
                    {
                        stat.SetAttribute("Min", sMinStat[statsortorder[y]]);
                        stat.SetAttribute("Max", sMaxStat[statsortorder[y]]);
                    }
                    else
                    {
                        stat.SetAttribute("Min","0");
                        stat.SetAttribute("Max","0");
                    }
                    girl.AppendChild(stat);
                }
                for (int y = 0; y < sSkills.Count(); y++)
                {
                    skill = xmldoc.CreateElement("Skill");
                    skill.SetAttribute("Name", sSkills[skillsortorder[y]]);
                    skill.SetAttribute("Min", sMinSkill[skillsortorder[y]]);
                    skill.SetAttribute("Max", sMaxSkill[skillsortorder[y]]);
                    girl.AppendChild(skill);
                }
                string sCatacombs = "";
                switch (sData.ReadLine())
                {
                    case "0": sCatacombs = "No"; break;
                    case "1": sCatacombs = "Yes"; break;
                }
                girl.SetAttribute("Catacomb", sCatacombs);
                int iTraitNum = Convert.ToInt32(sData.ReadLine());
                for (int y = 0; y < iTraitNum; y++)
                {
                    trait = xmldoc.CreateElement("Trait");
                    trait.SetAttribute("Name", sData.ReadLine());
                    trait.SetAttribute("Percent", sData.ReadLine());
                    girl.AppendChild(trait);
                }
                girls.AppendChild(girl);

                int test = 0;
                string filename = filepath + sName + ".rgirlsx";

                while (File.Exists(Convert.ToString(filename)) == true)
                {
                    test++;
                    if (!Directory.Exists(filepath + test.ToString())) Directory.CreateDirectory(filepath + test.ToString());
                    filename = filepath + test.ToString() + "\\" + sName + ".rgirlsx";
                }


                XmlWriterSettings settings = new XmlWriterSettings();
                settings.Indent = true;
                settings.NewLineOnAttributes = true;
                settings.IndentChars = "    ";
                XmlWriter xmlwrite = XmlWriter.Create(filename, settings);

                xmldoc.Save(xmlwrite);
                xmlwrite.Close();
            }

        }
        private void SaveIndividualItemsXML(string path)
        {
            for (int x = 0; x < ItemsCollection.Rows.Count; x++)
            {
                string filepath = path;
                XmlDocument xmldoc = new XmlDocument();

                XmlElement items = xmldoc.CreateElement("Items");
                XmlElement item = xmldoc.CreateElement("Item");
                XmlElement effect = xmldoc.CreateElement("Effect");

                item = xmldoc.CreateElement("Item");

                StringReader sData = new StringReader(ItemsCollection.Rows[x][1].ToString());

                string sName = ItemsCollection.Rows[x][0].ToString();
                string sDesc = sData.ReadLine();
                bool tempitem = false;
                string[] sValues = sData.ReadLine().Split(' ');

                switch (sValues[0])
                {
                    case "1": sValues[0] = "Ring"; break;
                    case "2": sValues[0] = "Dress"; break;
                    case "3": sValues[0] = "Shoes"; break;
                    case "4": sValues[0] = "Food"; break;
                    case "5": sValues[0] = "Necklace"; break;
                    case "6": sValues[0] = "Weapon"; break;
                    case "7": sValues[0] = "Makeup"; break;
                    case "8": sValues[0] = "Armor"; break;
                    case "9": sValues[0] = "Misc"; break;
                    case "10": sValues[0] = "Armband"; break;
                    case "11": sValues[0] = "Small Weapon"; break;
                    case "12": sValues[0] = "Underwear"; break;
                    case "13": sValues[0] = "Hat"; break;
                    case "14": sValues[0] = "Helmet"; break;
                    case "15": sValues[0] = "Glasses"; break;
                    case "16": sValues[0] = "Swimsuit"; break;
                    case "17": sValues[0] = "Combat Shoes"; break;
                    case "18": sValues[0] = "Shield"; break;
                }
                switch (sValues[2])
                {
                    case "0": sValues[2] = "None"; break;
                    case "1": sValues[2] = "AffectsAll"; break;
                    case "2": sValues[2] = "Temporary"; tempitem = true; break;
                }
                switch (sValues[4])
                {
                    case "0": sValues[4] = "Common"; break;
                    case "1": sValues[4] = "Shop50"; break;
                    case "2": sValues[4] = "Shop25"; break;
                    case "3": sValues[4] = "Shop05"; break;
                    case "4": sValues[4] = "Catacomb15"; break;
                    case "5": sValues[4] = "Catacomb05"; break;
                    case "6": sValues[4] = "Catacomb01"; break;
                    case "7": sValues[4] = "ScriptOnly"; break;
                    case "8": sValues[4] = "ScriptOrReward"; break;
                }
                switch (sValues[5])
                {
                    case "1": sValues[5] = "true"; break;
                    case "0":
                    default: sValues[5] = "false"; break;
                }
                item.SetAttribute("Name", sName);
                item.SetAttribute("Desc", sDesc);
                item.SetAttribute("Type", sValues[0]);
                item.SetAttribute("Badness", sValues[1]);
                item.SetAttribute("Special", sValues[2]);
                item.SetAttribute("Cost", sValues[3]);
                item.SetAttribute("Rarity", sValues[4]);
                item.SetAttribute("Infinite", sValues[5]);

                if (sValues.Length >= 7)  //check to see if item is old item that doesn't have girlbuychance
                {
                    item.SetAttribute("GirlBuyChance", sValues[6]);
                }
                else
                {
                    item.SetAttribute("GirlBuyChance", "0");
                }
                if (sValues.Length >= 8)  //check to see if item is old item that doesn't have girlbuychance
                {
                    item.SetAttribute("ItemWeight", sValues[7]);
                }
                else
                {
                    item.SetAttribute("ItemWeight", "0.0");
                }

                
                int num = Convert.ToInt32(sData.ReadLine());
                for (int y = 0; y < num; y++)
                {
                    effect = xmldoc.CreateElement("Effect");

                    string sEffectTest = sData.ReadLine();
                    string[] sEffects;

                    if (IsNumeric(sEffectTest) == true && Convert.ToInt32(sEffectTest) == 4)
                    {
                        sEffects = new string[4];
                        sEffects[0] = "Trait";
                        sEffects[1] = sData.ReadLine();
                        sEffects[2] = sData.ReadLine();
                        sEffects[3] = sData.ReadLine();
                    }
                    else
                    {
                        sEffects = sEffectTest.Split(' ');
                        switch (sEffects[0])
                        {
                            case "0":
                                sEffects[0] = "Skill";
                                switch (sEffects[1])
                                {
                                    case "0": sEffects[1] = "Anal"; break;
                                    case "1": sEffects[1] = "Magic"; break;
                                    case "2": sEffects[1] = "BDSM"; break;
                                    case "3": sEffects[1] = "NormalSex"; break;
                                    case "4": sEffects[1] = "Beastiality"; break;
                                    case "5": sEffects[1] = "Group"; break;
                                    case "6": sEffects[1] = "Lesbian"; break;
                                    case "7": sEffects[1] = "Service"; break;
                                    case "8": sEffects[1] = "Strip"; break;
                                    case "9": sEffects[1] = "Combat"; break;
                                    case "10": sEffects[1] = "OralSex"; break;
                                    case "11": sEffects[1] = "TittySex"; break;
                                    case "12": sEffects[1] = "Medicine"; break;
                                    case "13": sEffects[1] = "Performance"; break;
                                    case "14": sEffects[1] = "Handjob"; break;
                                    case "15": sEffects[1] = "Crafting"; break;
                                    case "16": sEffects[1] = "Herbalism"; break;
                                    case "17": sEffects[1] = "Farming"; break;
                                    case "18": sEffects[1] = "Brewing"; break;
                                    case "19": sEffects[1] = "AnimalHandling"; break;
                                    case "20": sEffects[1] = "Footjob"; break;
                                    case "21": sEffects[1] = "Cooking"; break;
                                }
                                break;
                            case "1":
                                sEffects[0] = "Stat";
                                switch (sEffects[1])
                                {
                                    case "0": sEffects[1] = "Charisma"; break;
                                    case "1": sEffects[1] = "Happiness"; break;
                                    case "2": sEffects[1] = "Libido"; break;
                                    case "3": sEffects[1] = "Constitution"; break;
                                    case "4": sEffects[1] = "Intelligence"; break;
                                    case "5": sEffects[1] = "Confidence"; break;
                                    case "6": sEffects[1] = "Mana"; break;
                                    case "7": sEffects[1] = "Agility"; break;
                                    case "8": sEffects[1] = "Fame"; break;
                                    case "9": sEffects[1] = "Level"; break;
                                    case "10": sEffects[1] = "AskPrice"; break;
                                    case "11": sEffects[1] = "House"; break;
                                    case "12": sEffects[1] = "Exp"; break;
                                    case "13": sEffects[1] = "Age"; break;
                                    case "14": sEffects[1] = "Obedience"; break;
                                    case "15": sEffects[1] = "Spirit"; break;
                                    case "16": sEffects[1] = "Beauty"; break;
                                    case "17": sEffects[1] = "Tiredness"; break;
                                    case "18": sEffects[1] = "Health"; break;
                                    case "19": sEffects[1] = "PCFear"; break;
                                    case "20": sEffects[1] = "PCLove"; break;
                                    case "21": sEffects[1] = "PCHate"; break;
                                    case "22": sEffects[1] = "Morality"; break;
                                    case "23": sEffects[1] = "Refinement"; break;
                                    case "24": sEffects[1] = "Dignity"; break;
                                    case "25": sEffects[1] = "Lactation"; break;
                                    case "26": sEffects[1] = "Strength"; break;
                                    case "27": sEffects[1] = "NPCLove"; break;
                                }
                                break;
                            case "2":
                                sEffects[0] = "Nothing";
                                break;
                            case "3":
                                sEffects[0] = "GirlStatus";
                                switch (sEffects[1])
                                {
                                    case "1": sEffects[1] = "Poisoned"; break;
                                    case "2": sEffects[1] = "Badly Poisoned"; break;
                                    case "3": sEffects[1] = "Pregnant"; break;
                                    case "4": sEffects[1] = "Pregnant By Player"; break;
                                    case "5": sEffects[1] = "Slave"; break;
                                    case "6": sEffects[1] = "Has Daughter"; break;
                                    case "7": sEffects[1] = "Has Son"; break;
                                    case "8": sEffects[1] = "Inseminated"; break;
                                    case "9": sEffects[1] = "Controlled"; break;
                                    case "10": sEffects[1] = "Catacombs"; break;
                                    case "11": sEffects[1] = "Arena"; break;
                                    case "12": sEffects[1] = "Your Daughter"; break;
                                    case "13": sEffects[1] = "Is Daughter"; break;
                                }
                                break;
                            case "4":
                                sEffects[0] = "Trait";
                                break;
                        }
                    }
                    effect.SetAttribute("What", sEffects[0]);
                    effect.SetAttribute("Name", sEffects[1]);
                    effect.SetAttribute("Amount", sEffects[2]);
                    if (sEffects[0] == "Trait" && tempitem)             // `J` only save duration for temporary traits.
                        effect.SetAttribute("Duration", sEffects[3]);

                    item.AppendChild(effect);
                }
                items.AppendChild(item);

                xmldoc.AppendChild(items);

                int test = 0;
                string filename = filepath + sName + ".itemsx";

                while (File.Exists(Convert.ToString(filename)) == true)
                {
                    test++;
                    if (!Directory.Exists(filepath + test.ToString())) Directory.CreateDirectory(filepath + test.ToString());
                    filename = filepath + test.ToString() + "\\" + sName + ".itemsx";
                }

                XmlWriterSettings settings = new XmlWriterSettings();
                settings.Indent = true;
                settings.NewLineOnAttributes = false;
                settings.IndentChars = "    ";
                XmlWriter xmlwrite = XmlWriter.Create(filename, settings);

                xmldoc.Save(xmlwrite);
                xmlwrite.Close();
            }
        }

        //*************************
        //*****   Items tab   *****
        //*************************

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            lTrack1.Text = trackBar_bad_02.Value.ToString();
        }

        private void comboBox_ItemDuration_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox_ItemDuration.SelectedIndex == 2)
            {
                if (dataGridView3.ColumnCount > 2) dataGridView3.Columns[2].Visible = true;
                textBox_Item_TempTraitDuration.Visible = true;
                label199.Visible = true;
            }
            else
            {
                if (dataGridView3.ColumnCount > 2) dataGridView3.Columns[2].Visible = false;
                textBox_Item_TempTraitDuration.Visible = false;
                label199.Visible = false;
            }
        }


        //Since some drop boxes have different contents in regard of previus one this is how to do it. Simply if index is this and this, clear droplist and fill these in
        //also, in status case there's no value, it's simply add or remove, so in case status is selected value textbox is hidden and add/remove droplist is displayed, and vice versa
        private void comboBox_affects_01_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox_affects_01.SelectedIndex == 0)			//Skills
            {
                affects_value_label.Visible = true;
                affects_status_label.Visible = false;
                affects_textBox_value.Visible = true;
                affects_comboBox_status.Visible = false;
                comboBox_affects_02.Items.Clear();
                comboBox_affects_02.Items.Add("Anal");
                comboBox_affects_02.Items.Add("Magic");
                comboBox_affects_02.Items.Add("BDSM");
                comboBox_affects_02.Items.Add("Normal Sex");
                comboBox_affects_02.Items.Add("Bestiality");
                comboBox_affects_02.Items.Add("Group");
                comboBox_affects_02.Items.Add("Lesbian");
                comboBox_affects_02.Items.Add("Service");
                comboBox_affects_02.Items.Add("Strip");
                comboBox_affects_02.Items.Add("Combat");
                comboBox_affects_02.Items.Add("OralSex");
                comboBox_affects_02.Items.Add("TittySex");
                comboBox_affects_02.Items.Add("Medicine");
                comboBox_affects_02.Items.Add("Performance");
                comboBox_affects_02.Items.Add("Handjob");
                comboBox_affects_02.Items.Add("Crafting");
                comboBox_affects_02.Items.Add("Herbalism");
                comboBox_affects_02.Items.Add("Farming");
                comboBox_affects_02.Items.Add("Brewing");
                comboBox_affects_02.Items.Add("AnimalHandling");
                comboBox_affects_02.Items.Add("Footjob");
                comboBox_affects_02.Items.Add("Cooking");
                affects_textBox_value.Text = "";
            }
            if (comboBox_affects_01.SelectedIndex == 1)			//Stats
            {
                affects_value_label.Visible = true;
                affects_status_label.Visible = false;
                affects_textBox_value.Visible = true;
                affects_comboBox_status.Visible = false;
                comboBox_affects_02.Items.Clear();
                comboBox_affects_02.Items.Add("Charisma");
                comboBox_affects_02.Items.Add("Happiness");
                comboBox_affects_02.Items.Add("Libedo");
                comboBox_affects_02.Items.Add("Constitution");
                comboBox_affects_02.Items.Add("Intelligence");
                comboBox_affects_02.Items.Add("Confidence");
                comboBox_affects_02.Items.Add("Mana");
                comboBox_affects_02.Items.Add("Agility");
                comboBox_affects_02.Items.Add("Fame");
                comboBox_affects_02.Items.Add("Level");
                comboBox_affects_02.Items.Add("AskPrice");
                comboBox_affects_02.Items.Add("House");
                comboBox_affects_02.Items.Add("Experience");
                comboBox_affects_02.Items.Add("Age");
                comboBox_affects_02.Items.Add("Obedience");
                comboBox_affects_02.Items.Add("Spirit");
                comboBox_affects_02.Items.Add("Beauty");
                comboBox_affects_02.Items.Add("Tiredness");
                comboBox_affects_02.Items.Add("Health");
                comboBox_affects_02.Items.Add("PC Fear");
                comboBox_affects_02.Items.Add("PC Love");
                comboBox_affects_02.Items.Add("PC Hate");
                comboBox_affects_02.Items.Add("Morality");
                comboBox_affects_02.Items.Add("Refinement");
                comboBox_affects_02.Items.Add("Dignity");
                comboBox_affects_02.Items.Add("Lactation");
                comboBox_affects_02.Items.Add("Strength");
                comboBox_affects_02.Items.Add("NPCLove");
                affects_textBox_value.Text = "";
            }
            if (comboBox_affects_01.SelectedIndex == 2)			//Status
            {
                affects_value_label.Visible = false;
                affects_status_label.Visible = true;
                affects_textBox_value.Visible = false;
                affects_comboBox_status.Visible = true;
                comboBox_affects_02.Items.Clear();
                comboBox_affects_02.Items.Add("Poisoned");
                comboBox_affects_02.Items.Add("Badly Poisoned");
                comboBox_affects_02.Items.Add("Pregnant");
                comboBox_affects_02.Items.Add("Pregnant By Player");
                comboBox_affects_02.Items.Add("Slave");
                comboBox_affects_02.Items.Add("Has daughter");
                comboBox_affects_02.Items.Add("Has son");
                comboBox_affects_02.Items.Add("Inseminated");
                comboBox_affects_02.Items.Add("Controlled");
                comboBox_affects_02.Items.Add("Catacombs");
                comboBox_affects_02.Items.Add("Arena");
                comboBox_affects_02.Items.Add("Your Daughter");
                comboBox_affects_02.Items.Add("Is Daughter");
            }

            /* `J` this is not really needed

            //not all combinations are possible, this takes care of that, duration is disabled in these cases

            //if consumables and skills, or consumables and stats are selected duration drop list is enabled
            if ((comboBox_affects_01.SelectedIndex == 0 && comboBox_ItemType_01.SelectedIndex == 3) || 
                (comboBox_affects_01.SelectedIndex == 1 && comboBox_ItemType_01.SelectedIndex == 3))		
            {
                comboBox_ItemDuration.Enabled = true;			//enables duration drop list in case it was disabled
            }
            else													//for other combinations (either item is not consumable, or it is consumable, but status is selected disable it and reset it's value
            {
                comboBox_ItemDuration.Enabled = false;				//item duration doesn't affect status changes so to be better safe than sorry we disable it and set it do default value
                comboBox_ItemDuration.SelectedIndex = 0;
            }
            */
        }

        //adds special effects to the item datagrid
        private void button_AddToItemDataGrid_Click(object sender, EventArgs e)
        {
            try
            {
                if (comboBox_affects_01.SelectedIndex == 2) iTable.Rows.Add(comboBox_affects_01.SelectedItem.ToString(), comboBox_affects_02.SelectedItem.ToString(), affects_comboBox_status.SelectedItem.ToString());
                else iTable.Rows.Add(comboBox_affects_01.SelectedItem.ToString(), comboBox_affects_02.SelectedItem.ToString(), affects_textBox_value.Text);
            }
            catch (Exception err)
            {
                if (comboBox_affects_01.SelectedItem == null)
                    dataGridView_Bad_Files.Rows.Add("Null string from button_AddToItemDataGrid_Click", err.Message);
                else
                    dataGridView_Bad_Files.Rows.Add(comboBox_affects_01.SelectedItem.ToString(), err.Message);

                MessageBox.Show("One or more drop fields are empty", "Add error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }
        //remove selected effect
        private void button_RemoveItemFromDataGrid_Click(object sender, EventArgs e)
        {
            try
            {
                iTable.Rows[dataGridView2.CurrentRow.Index].Delete();
            }
            catch (Exception err)
            {
                if (dataGridView2.CurrentRow == null)
                    dataGridView_Bad_Files.Rows.Add("Null string from button_RemoveItemFromDataGrid_Click", err.Message);
                else
                    dataGridView_Bad_Files.Rows.Add(dataGridView2.CurrentRow.Index, err.Message);
                MessageBox.Show("Nothing is selected or list is empty.", "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
            }
        }

        //adds trait to item
        private void button_AddTraitsToItem_Click(object sender, EventArgs e)
        {
            try
            {
                iTTable.Rows.Add(comboBox_ItemTraits.SelectedItem.ToString(), comboBox_ItemTraitsEffect.SelectedItem.ToString()
                    , textBox_Item_TempTraitDuration.Text.ToString()
                    );
            }
            catch (Exception err)
            {
                MessageBox.Show("One or more fields are empty.", "Add error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                string a=err.ToString();
            }
        }
        //remove selected trait
        private void button_RemoveTraitsFromItem_Click(object sender, EventArgs e)
        {
            try
            {
                iTTable.Rows[dataGridView3.CurrentRow.Index].Delete();
            }
            catch (Exception err)
            {
                //dataGridView_Bad_Files.Rows.Add(dataGridView3.CurrentRow.Index, err.Message);
                MessageBox.Show("Nothing is selected or list is empty.", "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                string a = err.ToString();
            }
        }

        //method that compiles values from items tab to game format
        private string[] CompileItem(string output)
        {
            string[] sItem = { "", "" };		//string array that will be returned, as usual, name at 0 index, data as 1 index
            string nln = "\r\n";
            string sp = " ";
            string sItemInfinite = "";			//item is infinite in shop?
            string sEffectNumber = "";			//number of effects added, and also traits added, no need for separate variable since it's used only once in each case
            string sEffects = "";				//string to store effects added when done
            string sTraits = "";				//string to store traits added when done
            string sDuration = "0";				//trait duration 

            sItemInfinite = (checkBox_infinite_06.Checked) ? "1" : "0";

            if (dataGridView2.RowCount == 0) sEffectNumber = "0";				//read item effects from datagrid and parse them to game format
            else
            {
                sEffectNumber = dataGridView2.RowCount.ToString();
                int i = 0;
                while (i < dataGridView2.RowCount)
                {
                    sEffects = sEffects + nln + EffectParse(dataGridView2.Rows[i].Cells[0].Value.ToString(), dataGridView2.Rows[i].Cells[1].Value.ToString(), dataGridView2.Rows[i].Cells[2].Value.ToString());
                    i++;
                }
            }

            if (dataGridView3.RowCount > 0)										//read item traits from datagrid and parse them to game format
            {
                sEffectNumber = (Convert.ToInt32(sEffectNumber) + dataGridView3.RowCount).ToString();
                int i = 0;
                while (i < dataGridView3.RowCount)
                {
                    sTraits = sTraits + nln + TraitsParse(dataGridView3.Rows[i].Cells[0].Value.ToString(), dataGridView3.Rows[i].Cells[1].Value.ToString(), dataGridView3.Rows[i].Cells[2].Value.ToString());
                    i++;
                }
            }

            if (comboBox_ItemDuration.SelectedIndex == 0) sDuration = "0";		//sets flag 3 to Default behaviour (permanent)
            if (comboBox_ItemDuration.SelectedIndex == 1) sDuration = "1";		//sets flag 3 to Affects all girls temporarily
            if (comboBox_ItemDuration.SelectedIndex == 2) sDuration = "2";		//sets flag 3 to Affects one girl temporarily

            //not sure what (if anything) this does, it seems that this is obsolete now as compile button passes only "list" parameter, or better said, in current app this doesn't do anything, but I left it here "just in case"
            if (output == "single") sItem[0] = textBox_ItemName.Text + nln + textBox_ItemDesc.Text + nln + (comboBox_ItemType_01.SelectedIndex + 1).ToString() + sp + trackBar_bad_02.Value.ToString() + sp + sDuration + sp + textBox_Item_Cost.Text + sp + comboBox_Rarity_05.SelectedIndex.ToString() + sp + sItemInfinite + sp + textBox_Item_Weight.Text + nln + sEffectNumber + sEffects + sTraits;
            else if (output == "list")
            {
                sItem[0] = textBox_ItemName.Text;
                if (checkBox_Item_GirlBuyChance.Checked == true) sItem[1] = textBox_ItemDesc.Text + nln + (comboBox_ItemType_01.SelectedIndex + 1).ToString() + sp + trackBar_bad_02.Value.ToString() + sp + sDuration + sp + textBox_Item_Cost.Text + sp + comboBox_Rarity_05.SelectedIndex.ToString() + sp + sItemInfinite + sp + textBox_Item_GirlBuyChance.Text + sp + textBox_Item_Weight.Text + nln + sEffectNumber + sEffects + sTraits;
                else sItem[1] = textBox_ItemDesc.Text + nln + (comboBox_ItemType_01.SelectedIndex + 1).ToString() + sp + trackBar_bad_02.Value.ToString() + sp + sDuration + sp + textBox_Item_Cost.Text + sp + comboBox_Rarity_05.SelectedIndex.ToString() + sp + sItemInfinite + sp + textBox_Item_Weight.Text + nln + sEffectNumber + sEffects + sTraits;
            }
            return sItem;
        }

        //adds item to item list (DataTable and ComboBox)
        private void button_Add_Item_To_List_Click(object sender, EventArgs e)
        {
            ItemsCollection.Rows.Add(CompileItem("list").ElementAt(0), CompileItem("list").ElementAt(1));
            SortDataTable(ref ItemsCollection, ref listBox_ItemsList, textBox_ItemName.Text);
            StatusLabel1.Text = "Added item " + "\"" + ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0] + "\"" + " to list...";
        }
        //replaces values at currently selected index in DataTable and ComboBox
        private void button_Update_Selected_Item_Click(object sender, EventArgs e)
        {
            ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0] = CompileItem("list").ElementAt(0);
            ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][1] = CompileItem("list").ElementAt(1);
            SortDataTable(ref ItemsCollection, ref listBox_ItemsList, textBox_ItemName.Text);
            StatusLabel1.Text = "Changed item entry No" + (listBox_ItemsList.SelectedIndex + 1).ToString() + " " + "(\"" + ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0] + "\")...";
        }
        //resets items tab
        private void button_Reset_Item_Click(object sender, EventArgs e)
        {
            textBox_ItemName.Text = "";
            textBox_ItemDesc.Text = "";
            comboBox_ItemType_01.SelectedIndex = 3;
            comboBox_Rarity_05.SelectedIndex = 0;
            comboBox_ItemDuration.SelectedIndex = 0;
            textBox_Item_Cost.Text = "10";
            textBox_Item_Weight.Text = "0.0";
            iTable.Clear();
            iTTable.Clear();
            affects_textBox_value.Text = "0";
            trackBar_bad_02.Value = 0;
            checkBox_infinite_06.Checked = false;
            lTrack1.Text = "0";
            comboBox_affects_01.SelectedIndex = -1;
            comboBox_affects_02.SelectedIndex = -1;
            comboBox_ItemTraits.SelectedIndex = -1;
            comboBox_ItemTraitsEffect.SelectedIndex = -1;
            affects_comboBox_status.SelectedIndex = -1;
            this.listBox_ItemsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_ItemsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
            listBox_ItemsList.SelectedItem = null;
            this.listBox_ItemsList.SelectedIndexChanged += new System.EventHandler(this.listBox_ItemsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
            StatusLabel1.Text = "Reset of item tab values performed...";
        }

        //parses effect values to game values, I started to use arrays with values that are in dropboxes, I think it's somewhat less work to do if checks through them instead of making mile long if statements, and definitely less code than a bunch of ifs, too bad i didn't think of this sooner :P
        private string EffectParse(string type, string attribute, string value)
        {
            string sEf01 = "";	//type
            string sEf02 = "";	//attribute
            string sEf03 = "";	//value

            string[,] aTypes = new string[3, 2] { { "Skill", "0" }, { "Stat", "1" }, { "GirlStatus", "3" } };
            string[,] aSkills = new string[22, 2] { { "Anal", "0" }, { "Magic", "1" }, { "BDSM", "2" }, { "Normal Sex", "3" }, { "Bestiality", "4" }, { "Group", "5" }, { "Lesbian", "6" }, { "Service", "7" }, { "Strip", "8" }, { "Combat", "9" }, { "OralSex", "10" }, { "TittySex", "11" }, { "Medicine", "12" }, { "Performance", "13" }, { "Handjob", "14" }, { "Crafting", "15" }, { "Herbalism", "16" }, { "Farming", "17" }, { "Brewing", "18" }, { "AnimalHandling", "19" }, { "Footjob", "20" } , { "Cooking", "21"}};
            string[,] aStats = new string[28, 2] { { "Charisma", "0" }, { "Happiness", "1" }, { "Libedo", "2" }, { "Constitution", "3" }, { "Intelligence", "4" }, { "Confidence", "5" }, { "Mana", "6" }, { "Agility", "7" }, { "Fame", "8" }, { "Level", "9" }, { "AskPrice", "10" }, { "House", "11" }, { "Experience", "12" }, { "Age", "13" }, { "Obedience", "14" }, { "Spirit", "15" }, { "Beauty", "16" }, { "Tiredness", "17" }, { "Health", "18" }, { "PC Fear", "19" }, { "PC Love", "20" }, { "PC Hate", "21" }, { "Morality", "22" }, { "Refinement", "23" }, { "Dignity", "24" }, { "Lactation", "25" }, { "Strength", "26" }, { "NPCLove", "27" } };
            string[,] aStatus = new string[13, 2] { { "Poisoned", "1" }, { "Badly Poisoned", "2" }, { "Pregnant", "3" }, { "Pregnant By Player", "4" }, { "Slave", "5" }, { "Has daughter", "6" }, { "Has son", "7" }, { "Inseminated", "8" }, { "Controlled", "9" }, { "Catacombs", "10" }, { "Arena", "11" }, { "Your Daughter", "12" }, { "Is Daughter", "13" } };

            //first check type against aTypes array, when it coresponds to one then we found our type, store number of that type to sEf01 string
            //that's why array's are two dimensional, easy to access value linked with text

            int i = 0;
            while (i < 3)
            {
                if (type == aTypes[i, 0])
                {
                    sEf01 = aTypes[i, 1];
                    break;
                }
                i++;
            }

            //now, depending on what type it is compare against it's respective array and read value linked to that attribute

            if (sEf01 == "0")
            {
                i = 0;
                while (i < aSkills.Length)
                {
                    if (attribute == aSkills[i, 0])
                    {
                        sEf02 = aSkills[i, 1];
                        break;
                    }
                    i++;
                }
            }
            else if (sEf01 == "1")
            {
                i = 0;
                while (i < aStats.Length)
                {
                    if (attribute == aStats[i, 0])
                    {
                        sEf02 = aStats[i, 1];
                        break;
                    }
                    i++;
                }
            }
            else if (sEf01 == "3")
            {
                i = 0;
                while (i < aStatus.Length)
                {
                    if (attribute == aStatus[i, 0])
                    {
                        sEf02 = aStatus[i, 1];
                        break;
                    }
                    i++;
                }
            }

            //these do not need arrays, for skill and stat value is already entered, for status we just need to check if it's Add or not and "act" accordingly

            if (sEf01 == "0" || sEf01 == "1") sEf03 = value;
            else if (sEf01 == "3")
            {
                if (value == "Add") sEf03 = "1";
                else sEf03 = "0";
            }

            return (sEf01 + " " + sEf02 + " " + sEf03);		//and in the end return finished effect in game format (number number number)
        }
        //parses traits to game values
        private string TraitsParse(string trait, string value, string duration="20")
        {
            //traits are much simpler than items, they're represented by their names so there's no need to assign numbers, just take the trait and check if it's add or remove and apply that
            string sTr = "";
            sTr = (value == "Add") ? "1" : "0";
            return ("4" + "\r\n" + trait + "\r\n" + sTr + "\r\n" + duration);
        }

        //toggles readonly attribute for value field in datagridview
        private void checkBox_DG2_CheckedChanged(object sender, EventArgs e)
        {
            dataGridView2.Columns[2].ReadOnly = (checkBox_DG2.Checked) ? true : false;
        }
        //toggles readonly attribute for value field in datagridview	
        private void checkBox_DG3_CheckedChanged(object sender, EventArgs e)
        {
            dataGridView3.Columns[1].ReadOnly = (checkBox_DG3.Checked) ? true : false;
            dataGridView3.Columns[2].ReadOnly = (checkBox_DG3.Checked) ? true : false;
        }

        //disables duration drop list if item type selected is not consumable
        private void comboBox_ItemType_01_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (comboBox_ItemType_01.SelectedIndex == 3 && comboBox_affects_01.SelectedIndex != 2)
            {
                comboBox_ItemDuration.Enabled = true;
            }
            else
            {
                comboBox_ItemDuration.Enabled = false;
                comboBox_ItemDuration.SelectedIndex = 0;
            }
        }

        //takes care of disabling GirlBuyChance value when checkbox is cleared
        private void checkBox_Item_GirlBuyChance_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox_Item_GirlBuyChance.Checked == true)
            {
                textBox_Item_GirlBuyChance.Enabled = true;
                if (textBox_Item_GirlBuyChance.Text.Length == 0) textBox_Item_GirlBuyChance.Text = "0";
            }
            else textBox_Item_GirlBuyChance.Enabled = false;
        }

        //button that loads existing items file
        private void button_ItemLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog Open = new OpenFileDialog();
            Open.Filter = "Whore Master items File|*.itemsx; *.items|All files|*.*";
            Open.ShowDialog();

            switch (Path.GetExtension(Open.FileName))
            {
                case ".items":
                    LoadItems(Open.FileName);
                    break;
                case ".itemsx":
                    LoadItemsXML(Open.FileName);
                    break;
            }
            SortDataTable(ref ItemsCollection, ref listBox_ItemsList);
        }
        //method that parses items files to fill ComboBox and DataTable
        private void LoadItems(string path)
        {
            try
            {
                int x = 0;
                StreamReader Import = new StreamReader(path);
                while (Import.Peek() >= 0)
                {
                    x++;
                    string name = Convert.ToString(Import.ReadLine());
                    string desc = Convert.ToString(Import.ReadLine());
                    string data = Convert.ToString(Import.ReadLine());
                    string num = Convert.ToString(Import.ReadLine());
                    string temp = "";

                    for (int i = 0; i < Convert.ToInt32(num); i++)
                    {
                        string test = Convert.ToString(Import.ReadLine());

                        if (IsNumeric(test) == true && Convert.ToInt32(test) == 4)
                        {
                            test = test + "\r\n" + Convert.ToString(Import.ReadLine()) + "\r\n" + Convert.ToString(Import.ReadLine());
                        }
                        temp = temp + test + "\r\n";
                    }

                    listBox_ItemsList.Items.Add(name);
                    ItemsCollection.Rows.Add(name, (desc + "\r\n" + data + "\r\n" + num + "\r\n" + temp).TrimEnd('\r', '\n'));
                }
                Import.Close();
                StatusLabel1.Text = "Loaded " + x.ToString() + " items from file...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("File load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "Open error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "Load canceled...";
            }
        }
        //method that parses XML items files, and translates them to the old format (because I'm lazy to rework other methods to work natively with XML :P)
        private void LoadItemsXML(string path)
        {
            try
            {
                XmlTextReader xmlread = new XmlTextReader(path);
                XmlDocument xmldoc = new XmlDocument();
                xmldoc.Load(xmlread);
                XmlNode baseNode = xmldoc.DocumentElement;

                int z = 0;
                foreach (XmlNode node in baseNode.SelectNodes("/Items/Item"))
                {
                    z++;
                    string sName = node.Attributes["Name"].Value;				//item name
                    string sDesc = node.Attributes["Desc"].Value;				//item description
                    string sData = "";											//string that will store item data
                    ArrayList alEffects = new ArrayList();						//ArrayList to store parsed effects

                    switch (node.Attributes["Type"].Value)						//testing for item type, 1st number in "old" format
                    {
                        case "Ring": sData = "1"; break;
                        case "Dress": sData = "2"; break;
                        case "Shoes": sData = "3"; break;
                        case "Food": sData = "4"; break;
                        case "Necklace": sData = "5"; break;
                        case "Weapon": sData = "6"; break;
                        case "Makeup": sData = "7"; break;
                        case "Armor": sData = "8"; break;
                        case "Armband": sData = "10"; break;
                        case "Small Weapon": sData = "11"; break;
                        case "Under Wear":
                        case "Underwear": sData = "12"; break;
                        case "Hat": sData = "13"; break;
                        case "Helmet": sData = "14"; break;
                        case "Glasses": sData = "15"; break;
                        case "Swimsuit": sData = "16"; break;
                        case "Combat Shoes": sData = "17"; break;
                        case "Shield": sData = "18"; break;
                        case "Misc":
                        default: sData = "9"; break;
                    }
                    sData = sData + " " + node.Attributes["Badness"].Value;		//item "badness" value, 2nd value
                    switch (node.Attributes["Special"].Value)					//item special value, how it effects girl(s), 3nd value
                    {
                        case "AffectsAll": sData = sData + " " + "1"; break;
                        case "Temporary": sData = sData + " " + "2"; break;
                        case "None":
                        default: sData = sData + " " + "0"; break;
                    }
                    sData = sData + " " + node.Attributes["Cost"].Value;			//item cost, 4th value
                    switch (node.Attributes["Rarity"].Value)						//item rarity, 5th value
                    {
                        case "Uncommon":
                        case "Shop50": sData = sData + " " + "1"; break;
                        case "Shop25": sData = sData + " " + "2"; break;
                        case "Shop05": sData = sData + " " + "3"; break;
                        case "Catacomb15": sData = sData + " " + "4"; break;
                        case "Catacomb05": sData = sData + " " + "5"; break;
                        case "Catacomb01": sData = sData + " " + "6"; break;
                        case "ScriptOnly": sData = sData + " " + "7"; break;
                        case "ScriptOrReward": sData = sData + " " + "8"; break;
                        case "Common":
                        default: sData = sData + " " + "0"; break;
                    }
                    switch (node.Attributes["Infinite"].Value)					//item is infinite, 6th value
                    {
                        case "true": sData = sData + " " + "1"; break;
                        case "false":
                        default: sData = sData + " " + "0"; break;
                    }
                    if (node.Attributes["GirlBuyChance"] != null)				//this will be 7th value, it doesn't exist in original items
                    {
                        sData = sData + " " + node.Attributes["GirlBuyChance"].Value;
                    }
                    else
                    {
                        sData = sData + " " + "0";
                    }
                    if (node.Attributes["ItemWeight"] != null)				//this will be 8th value, it doesn't exist in original items
                    {
                        sData = sData + " " + node.Attributes["ItemWeight"].Value;
                    }
                    else
                    {
                        sData = sData + " " + "0.0";
                    }
                    if (node.HasChildNodes == true)
                    {
                        for (int x = 0; x < node.ChildNodes.Count; x++)
                        {
                            string sEffect = "";
                            if (node.ChildNodes[x].NodeType != XmlNodeType.Element) continue;   // `J` only process elements
                            switch (node.ChildNodes[x].Attributes["What"].Value)	//what item affects, 1st of 3 numbers that define items
                            {
                                case "Skill":
                                    sEffect = "0";
                                    switch (node.ChildNodes[x].Attributes["Name"].Value)		//2nd number, in this case what skill it affects, I have to nest them since each of the types has different choices (i.e. anal as skill and intelligence as stat) so they won't get mixed
                                    {
                                        case "Anal": sEffect = sEffect + " " + "0"; break;
                                        case "Magic": sEffect = sEffect + " " + "1"; break;
                                        case "BDSM": sEffect = sEffect + " " + "2"; break;
                                        case "NormalSex": sEffect = sEffect + " " + "3"; break;
                                        case "Beastiality": sEffect = sEffect + " " + "4"; break;
                                        case "Group": sEffect = sEffect + " " + "5"; break;
                                        case "Lesbian": sEffect = sEffect + " " + "6"; break;
                                        case "Service": sEffect = sEffect + " " + "7"; break;
                                        case "Strip": sEffect = sEffect + " " + "8"; break;
                                        case "Combat": sEffect = sEffect + " " + "9"; break;
                                        case "OralSex": sEffect = sEffect + " " + "10"; break;
                                        case "TittySex": sEffect = sEffect + " " + "11"; break;
                                        case "Medicine": sEffect = sEffect + " " + "12"; break;
                                        case "Performance": sEffect = sEffect + " " + "13"; break;
                                        case "Handjob": sEffect = sEffect + " " + "14"; break;
                                        case "Crafting": sEffect = sEffect + " " + "15"; break;
                                        case "Herbalism": sEffect = sEffect + " " + "16"; break;
                                        case "Farming": sEffect = sEffect + " " + "17"; break;
                                        case "Brewing": sEffect = sEffect + " " + "18"; break;
                                        case "AnimalHandling": sEffect = sEffect + " " + "19"; break;
                                        case "Footjob": sEffect = sEffect + " " + "20"; break;
                                        case "Cooking": sEffect = sEffect + " " + "21"; break;
                                    }
                                    sEffect = sEffect + " " + node.ChildNodes[x].Attributes["Amount"].Value;
                                    break;
                                case "Stat":
                                    sEffect = "1";
                                    switch (node.ChildNodes[x].Attributes["Name"].Value)
                                    {
                                        case "Charisma": sEffect = sEffect + " " + "0"; break;
                                        case "Happiness": sEffect = sEffect + " " + "1"; break;
                                        case "Libido": sEffect = sEffect + " " + "2"; break;
                                        case "Constitution": sEffect = sEffect + " " + "3"; break;
                                        case "Intelligence": sEffect = sEffect + " " + "4"; break;
                                        case "Confidence": sEffect = sEffect + " " + "5"; break;
                                        case "Mana": sEffect = sEffect + " " + "6"; break;
                                        case "Agility": sEffect = sEffect + " " + "7"; break;
                                        case "Fame": sEffect = sEffect + " " + "8"; break;
                                        case "Level": sEffect = sEffect + " " + "9"; break;
                                        case "AskPrice": sEffect = sEffect + " " + "10"; break;
                                        case "House": sEffect = sEffect + " " + "11"; break;
                                        case "Exp": sEffect = sEffect + " " + "12"; break;
                                        case "Age": sEffect = sEffect + " " + "13"; break;
                                        case "Obedience": sEffect = sEffect + " " + "14"; break;
                                        case "Spirit": sEffect = sEffect + " " + "15"; break;
                                        case "Beauty": sEffect = sEffect + " " + "16"; break;
                                        case "Tiredness": sEffect = sEffect + " " + "17"; break;
                                        case "Health": sEffect = sEffect + " " + "18"; break;
                                        case "PCFear": sEffect = sEffect + " " + "19"; break;
                                        case "PCLove": sEffect = sEffect + " " + "20"; break;
                                        case "PCHate": sEffect = sEffect + " " + "21"; break;
                                        case "Morality": sEffect = sEffect + " " + "22"; break;
                                        case "Refinement": sEffect = sEffect + " " + "23"; break;
                                        case "Dignity": sEffect = sEffect + " " + "24"; break;
                                        case "Lactation": sEffect = sEffect + " " + "25"; break;
                                        case "Strength": sEffect = sEffect + " " + "26"; break;
                                        case "NPCLove": sEffect = sEffect + " " + "27"; break;
                                    }
                                    sEffect = sEffect + " " + node.ChildNodes[x].Attributes["Amount"].Value;
                                    break;
                                case "Nothing":
                                    sEffect = "2";
                                    break;
                                case "GirlStatus":
                                    sEffect = "3";
                                    switch (node.ChildNodes[x].Attributes["Name"].Value)
                                    {
                                        case "Poisoned": sEffect = sEffect + " " + "1"; break;
                                        case "Badly Poisoned": sEffect = sEffect + " " + "2"; break;
                                        case "Pregnant": sEffect = sEffect + " " + "3"; break;
                                        case "Pregnant By Player": sEffect = sEffect + " " + "4"; break;
                                        case "Slave": sEffect = sEffect + " " + "5"; break;
                                        case "Has Daughter": sEffect = sEffect + " " + "6"; break;
                                        case "Has Son": sEffect = sEffect + " " + "7"; break;
                                        case "Inseminated": sEffect = sEffect + " " + "8"; break;
                                        case "Controlled": sEffect = sEffect + " " + "9"; break;
                                        case "Catacombs": sEffect = sEffect + " " + "10"; break;
                                        case "Arena": sEffect = sEffect + " " + "11"; break;
                                        case "Your Daughter": sEffect = sEffect + " " + "12"; break;
                                        case "Is Daughter": sEffect = sEffect + " " + "13"; break;
                                    }
                                    sEffect = sEffect + " " + node.ChildNodes[x].Attributes["Amount"].Value;
                                    break;
                                case "Trait":
                                    string duration="20";
                                    if (node.ChildNodes[x].Attributes["Duration"] != null)
                                        duration = node.ChildNodes[x].Attributes["Duration"].Value;

                                    sEffect = "4" + "\r\n" + node.ChildNodes[x].Attributes["Name"].Value + "\r\n" + 
                                        node.ChildNodes[x].Attributes["Amount"].Value + "\r\n" + 
                                        duration;
                                    break;
                                default:
                                    break;
                            }
                            alEffects.Add(sEffect);
                        }
                    }

                    int num = alEffects.Count;
                    string sEffects = "";
                    for (int x = 0; x < num; x++)
                    {
                        if (num == x + 1) sEffects = sEffects + alEffects[x].ToString();
                        else sEffects = sEffects + alEffects[x].ToString() + "\r\n";
                    }

                    ItemsCollection.Rows.Add(sName, sDesc + "\r\n" + sData + "\r\n" + num.ToString() + "\r\n" + sEffects);
                    listBox_ItemsList.Items.Add(sName);
                }
                xmlread.Close();
                StatusLabel1.Text = "Loaded " + z.ToString() + " items from XML file...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("File load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "XML load error error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "XML load error...";
            }
        }
        //button that deletes selected item from list
        private void button_ItemDelete_Click(object sender, EventArgs e)
        {
            try
            {
                this.listBox_ItemsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_ItemsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
                StatusLabel1.Text = "\"" + listBox_ItemsList.SelectedItem + "\"" + " deleted...";
                ItemsCollection.Rows[listBox_ItemsList.SelectedIndex].Delete();
                listBox_ItemsList.Items.RemoveAt(listBox_ItemsList.SelectedIndex);
                this.listBox_ItemsList.SelectedIndexChanged += new System.EventHandler(this.listBox_ItemsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(listBox_ItemsList.SelectedIndex, err.Message);
                MessageBox.Show("It's probably nothing serious, i.e. you pressed Delete button\n\rwhile list was empty or nothing was selected, now for \"official\" error:\n\r\n\r" + err.Message, "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "Delete item error...";
            }
        }

        //this happens when you select something from item list listbox, that invokes SelectedIndexChanged event and values from entry with this index get filled to item tab boxes
        private void listBox_ItemsList_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (listBox_ItemsList.SelectedIndex < 0) return;
            string sEf01 = "";
            string sEf02 = "";
            string sEf03 = "";

            string[,] aTypes = new string[3, 2] { { "Skill", "0" }, { "Stat", "1" }, { "GirlStatus", "3" } };
            string[,] aSkills = new string[22, 2] { { "Anal", "0" }, { "Magic", "1" }, { "BDSM", "2" }, { "Normal Sex", "3" }, { "Bestiality", "4" }, { "Group", "5" }, { "Lesbian", "6" }, { "Service", "7" }, { "Strip", "8" }, { "Combat", "9" }, { "OralSex", "10" }, { "TittySex", "11" }, { "Medicine", "12" }, { "Performance", "13" }, { "Handjob", "14" }, { "Crafting", "15" }, { "Herbalism", "16" }, { "Farming", "17" }, { "Brewing", "18" }, { "AnimalHandling", "19" }, { "Footjob", "20" }, { "Cooking","21"} };
            string[,] aStats = new string[28, 2] { { "Charisma", "0" }, { "Happiness", "1" }, { "Libedo", "2" }, { "Constitution", "3" }, { "Intelligence", "4" }, { "Confidence", "5" }, { "Mana", "6" }, { "Agility", "7" }, { "Fame", "8" }, { "Level", "9" }, { "AskPrice", "10" }, { "House", "11" }, { "Experience", "12" }, { "Age", "13" }, { "Obedience", "14" }, { "Spirit", "15" }, { "Beauty", "16" }, { "Tiredness", "17" }, { "Health", "18" }, { "PC Fear", "19" }, { "PC Love", "20" }, { "PC Hate", "21" }, { "Morality", "22" }, { "Refinement", "23" }, { "Dignity", "24" }, { "Lactation", "25" }, { "Strength", "26" }, { "NPCLove", "27" } };
            string[,] aStatus = new string[13, 2] { { "Poisoned", "1" }, { "Badly Poisoned", "2" }, { "Pregnant", "3" }, { "Pregnant By Player", "4" }, { "Slave", "5" }, { "Has daughter", "6" }, { "Has son", "7" }, { "Inseminated", "8" }, { "Controlled", "9" }, { "Catacombs", "10" }, { "Arena", "11" }, { "Your Daughter", "12" }, { "Is Daughter", "13" } };


            textBox_ItemName.Text = ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0].ToString();	//name is already isolated so it's trivial to assign it to required textbox, it's in first cell ([0]) of datatable, only selected index is required, and since combobox is synchronised (or at least should be synchronized) with datatable it's easy to get name out
            string sData = ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][1].ToString();				//this where it gets complicated, item data is in separate cell (this time [1]) from name so we need to parse it, first we'll put it in temp string, and after that disassemble it. Since this data isn't fixed length it would prove too complicated (my guess would be more to the line of impossible when considering DataTable nature) to put each line in separate column

            StringReader data = new StringReader(sData);							//item data is split by lines, so we pull it through StringReader to ease reading it line by line

            textBox_ItemDesc.Text = data.ReadLine();								//first line of data string is description so that's goes straight to description textbox
            string sIDesc = data.ReadLine();										//next line is item data line, this one needs to be parsed so it goes to string to wait disection

            char[] separator = { ' ' };											//data in item data is space delimited, so to begin extraction from it we create an array of separator characters
            string[] values = sIDesc.Split(separator);							//use Split function to put every value that's "between" spaces to array
            //now that we have all 6 of required values safely tucked in their array spot we assign each value to it's place on item tab
            if (int.Parse(values[0]) < 1 || int.Parse(values[0]) > 18) comboBox_ItemType_01.SelectedIndex = 3;	//Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Food/Consumable" if value is out of bounds
            else comboBox_ItemType_01.SelectedIndex = int.Parse(values[0]) - 1;	//item type comboBox, it's "value - 1" because combobox index' go from 0 to 9, and item type index in game goes from 1 to 10, so to lower it to editor level... similarly when parsing items from tab values there's +1 for item type value

            trackBar_bad_02.Value = int.Parse(values[1]);						//how bad is item for the girl trackbar
            lTrack1.Text = values[1];											//label doesn't pick it up automaticaly when trackbar is set via program, so it also needs to be assign

            if (int.Parse(values[2]) < 0 || int.Parse(values[2]) > 2) comboBox_ItemDuration.SelectedIndex = 0;	//Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Permanent" if value is out of bounds
            else comboBox_ItemDuration.SelectedIndex = int.Parse(values[2]);		//old unused "flag 3", now duration

            textBox_Item_Cost.Text = values[3];								//item cost, just throw the string to textbox

            if (int.Parse(values[4]) < 0 || int.Parse(values[4]) > 8) comboBox_Rarity_05.SelectedIndex = 3;		//Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Common" if value is out of bounds
            comboBox_Rarity_05.SelectedIndex = int.Parse(values[4]);				//rarity comboBox, unlike item type these go from 0 to 6, like index count for comboBox object

            if (values[5] == "0") checkBox_infinite_06.Checked = false;			//item is infinite in marketplace or not, simple check and simple "switch"
            else checkBox_infinite_06.Checked = true;

            if (values.Length >= 7)     //GirlBuyChance value
            {
                checkBox_Item_GirlBuyChance.Checked = true;
                textBox_Item_GirlBuyChance.Text = values[6];
            }
            else
            {
                checkBox_Item_GirlBuyChance.Checked = false;
                textBox_Item_GirlBuyChance.Text = "0";
            }
            if (values.Length >= 8)     //Item Weight value
            {
                textBox_Item_Weight.Text = values[7];
            }
            else
            {
                textBox_Item_Weight.Text = "0.0";
            }

            int iEffNum = Convert.ToInt32(data.ReadLine());

            iTTable.Clear();
            iTable.Clear();

            for (int i = 0; i < iEffNum; i++)
            {
                string test = data.ReadLine();
                if (IsNumeric(test) == true && Convert.ToInt32(test) == 4)
                {
                    string trait = data.ReadLine();
                    string change = data.ReadLine();
                    string duration = data.ReadLine();
                    iTTable.Rows.Add(trait, (change == "1") ? "Add" : "Remove", duration);
                }
                else
                {
                    values = test.Split(separator);
                    int j = 0;
                    while (j < 3)
                    {
                        if (values[0] == aTypes[j, 1])
                        {
                            sEf01 = aTypes[j, 0];
                            break;
                        }
                        else j = j + 1;
                    }

                    if (values[0] == "0")   // skill
                    {
                        j = 0;
                        while (j < aSkills.Length)
                        {
                            if (values[1] == aSkills[j, 1])
                            {
                                sEf02 = aSkills[j, 0];
                                break;
                            }
                            else j = j + 1;
                        }
                    }
                    else if (values[0] == "1")  // stat
                    {
                        j = 0;
                        while (j < aStats.Length)
                        {
                            if (values[1] == aStats[j, 1])
                            {
                                sEf02 = aStats[j, 0];
                                break;
                            }
                            else j = j + 1;
                        }
                    }
                    else if (values[0] == "3") // status
                    {
                        j = 0;
                        while (j < aStatus.Length)
                        {
                            if (values[1] == aStatus[j, 1])
                            {
                                sEf02 = aStatus[j, 0];
                                break;
                            }
                            else j = j + 1;
                        }
                    }

                    if (values[0] == "0" || values[0] == "1") sEf03 = values[2];
                    else if (values[0] == "3")
                    {
                        sEf03 = (values[2] == "1") ? "Add" : "Remove";
                    }
                    iTable.Rows.Add(sEf01, sEf02, sEf03);
                }
            }
            StatusLabel1.Text = "\"" + textBox_ItemName.Text + "\"" + " parsed successfully...";
        }

        //button that compiles items data back from dataTable to output tab
        private void button_SaveItem_Click(object sender, EventArgs e)
        {
            Filesave.FileName = "";
            Filesave.Filter = "Whore Master XML items file|*.itemsx|All files|*.*";
            Filesave.DefaultExt = ".itemsx";
            try
            {
                Filesave.ShowDialog();
                if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
                if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");
                SaveItemsXML(Filesave.FileName);
                StatusLabel1.Text = "Successfully compiled " + listBox_ItemsList.Items.Count.ToString() + " items...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(Filesave.FileName, err.Message);
                StatusLabel1.Text = "Save Error.";
            }
        }
        //save in XML format
        private void SaveItemsXML(string path)
        {
            XmlDocument xmldoc = new XmlDocument();

            XmlElement items = xmldoc.CreateElement("Items");
            XmlElement item = xmldoc.CreateElement("Item");
            XmlElement effect = xmldoc.CreateElement("Effect");

            for (int x = 0; x < ItemsCollection.Rows.Count; x++)
            {
                item = xmldoc.CreateElement("Item");

                StringReader sData = new StringReader(ItemsCollection.Rows[x][1].ToString());

                string sName = ItemsCollection.Rows[x][0].ToString();
                string sDesc = sData.ReadLine();
                bool tempitem = false;
                string[] sValues = sData.ReadLine().Split(' ');

                switch (sValues[0])
                {
                    case "1": sValues[0] = "Ring"; break;
                    case "2": sValues[0] = "Dress"; break;
                    case "3": sValues[0] = "Shoes"; break;
                    case "4": sValues[0] = "Food"; break;
                    case "5": sValues[0] = "Necklace"; break;
                    case "6": sValues[0] = "Weapon"; break;
                    case "7": sValues[0] = "Makeup"; break;
                    case "8": sValues[0] = "Armor"; break;
                    case "9": sValues[0] = "Misc"; break;
                    case "10": sValues[0] = "Armband"; break;
                    case "11": sValues[0] = "Small Weapon"; break;
                    case "12": sValues[0] = "Underwear"; break;
                    case "13": sValues[0] = "Hat"; break;
                    case "14": sValues[0] = "Helmet"; break;
                    case "15": sValues[0] = "Glasses"; break;
                    case "16": sValues[0] = "Swimsuit"; break;
                    case "17": sValues[0] = "Combat Shoes"; break;
                    case "18": sValues[0] = "Shield"; break;
                }
                switch (sValues[2])
                {
                    case "0": sValues[2] = "None"; break;
                    case "1": sValues[2] = "AffectsAll"; break;
                    case "2": sValues[2] = "Temporary"; tempitem = true; break;
                }
                switch (sValues[4])
                {
                    case "0": sValues[4] = "Common"; break;
                    case "1": sValues[4] = "Shop50"; break;
                    case "2": sValues[4] = "Shop25"; break;
                    case "3": sValues[4] = "Shop05"; break;
                    case "4": sValues[4] = "Catacomb15"; break;
                    case "5": sValues[4] = "Catacomb05"; break;
                    case "6": sValues[4] = "Catacomb01"; break;
                    case "7": sValues[4] = "ScriptOnly"; break;
                    case "8": sValues[4] = "ScriptOrReward"; break;
                }
                switch (sValues[5])
                {
                    case "1": sValues[5] = "true"; break;
                    case "0":
                    default: sValues[5] = "false"; break;
                }
                item.SetAttribute("Name", sName);
                item.SetAttribute("Desc", sDesc);
                item.SetAttribute("Type", sValues[0]);
                item.SetAttribute("Badness", sValues[1]);
                item.SetAttribute("Special", sValues[2]);
                item.SetAttribute("Cost", sValues[3]);
                item.SetAttribute("Rarity", sValues[4]);
                item.SetAttribute("Infinite", sValues[5]);

                if (sValues.Length >= 7)  //check to see if item is old item that doesn't have girlbuychance
                {
                    item.SetAttribute("GirlBuyChance", sValues[6]);
                }
                else
                {
                    item.SetAttribute("GirlBuyChance", "0");
                }
                if (sValues.Length >= 8)  //check to see if item is old item that doesn't have girlbuychance
                {
                    item.SetAttribute("ItemWeight", sValues[7]);
                }
                else
                {
                    item.SetAttribute("ItemWeight", "0.0");
                }

                
                int num = Convert.ToInt32(sData.ReadLine());
                for (int y = 0; y < num; y++)
                {
                    effect = xmldoc.CreateElement("Effect");

                    string sEffectTest = sData.ReadLine();
                    string[] sEffects;

                    if (IsNumeric(sEffectTest) == true && Convert.ToInt32(sEffectTest) == 4)
                    {
                        sEffects = new string[4];
                        sEffects[0] = "Trait";
                        sEffects[1] = sData.ReadLine();
                        sEffects[2] = sData.ReadLine();
                        sEffects[3] = sData.ReadLine();
                    }
                    else
                    {
                        sEffects = sEffectTest.Split(' ');
                        switch (sEffects[0])
                        {
                            case "0":
                                sEffects[0] = "Skill";
                                switch (sEffects[1])
                                {
                                    case "0": sEffects[1] = "Anal"; break;
                                    case "1": sEffects[1] = "Magic"; break;
                                    case "2": sEffects[1] = "BDSM"; break;
                                    case "3": sEffects[1] = "NormalSex"; break;
                                    case "4": sEffects[1] = "Beastiality"; break;
                                    case "5": sEffects[1] = "Group"; break;
                                    case "6": sEffects[1] = "Lesbian"; break;
                                    case "7": sEffects[1] = "Service"; break;
                                    case "8": sEffects[1] = "Strip"; break;
                                    case "9": sEffects[1] = "Combat"; break;
                                    case "10": sEffects[1] = "OralSex"; break;
                                    case "11": sEffects[1] = "TittySex"; break;
                                    case "12": sEffects[1] = "Medicine"; break;
                                    case "13": sEffects[1] = "Performance"; break;
                                    case "14": sEffects[1] = "Handjob"; break;
                                    case "15": sEffects[1] = "Crafting"; break;
                                    case "16": sEffects[1] = "Herbalism"; break;
                                    case "17": sEffects[1] = "Farming"; break;
                                    case "18": sEffects[1] = "Brewing"; break;
                                    case "19": sEffects[1] = "AnimalHandling"; break;
                                    case "20": sEffects[1] = "Footjob"; break;
                                    case "21": sEffects[1] = "Cooking"; break;
                                }
                                break;
                            case "1":
                                sEffects[0] = "Stat";
                                switch (sEffects[1])
                                {
                                    case "0": sEffects[1] = "Charisma"; break;
                                    case "1": sEffects[1] = "Happiness"; break;
                                    case "2": sEffects[1] = "Libido"; break;
                                    case "3": sEffects[1] = "Constitution"; break;
                                    case "4": sEffects[1] = "Intelligence"; break;
                                    case "5": sEffects[1] = "Confidence"; break;
                                    case "6": sEffects[1] = "Mana"; break;
                                    case "7": sEffects[1] = "Agility"; break;
                                    case "8": sEffects[1] = "Fame"; break;
                                    case "9": sEffects[1] = "Level"; break;
                                    case "10": sEffects[1] = "AskPrice"; break;
                                    case "11": sEffects[1] = "House"; break;
                                    case "12": sEffects[1] = "Exp"; break;
                                    case "13": sEffects[1] = "Age"; break;
                                    case "14": sEffects[1] = "Obedience"; break;
                                    case "15": sEffects[1] = "Spirit"; break;
                                    case "16": sEffects[1] = "Beauty"; break;
                                    case "17": sEffects[1] = "Tiredness"; break;
                                    case "18": sEffects[1] = "Health"; break;
                                    case "19": sEffects[1] = "PCFear"; break;
                                    case "20": sEffects[1] = "PCLove"; break;
                                    case "21": sEffects[1] = "PCHate"; break;
                                    case "22": sEffects[1] = "Morality"; break;
                                    case "23": sEffects[1] = "Refinement"; break;
                                    case "24": sEffects[1] = "Dignity"; break;
                                    case "25": sEffects[1] = "Lactation"; break;
                                    case "26": sEffects[1] = "Strength"; break;
                                    case "27": sEffects[1] = "NPCLove"; break;
                                }
                                break;
                            case "2":
                                sEffects[0] = "Nothing";
                                break;
                            case "3":
                                sEffects[0] = "GirlStatus";
                                switch (sEffects[1])
                                {
                                    case "1": sEffects[1] = "Poisoned"; break;
                                    case "2": sEffects[1] = "Badly Poisoned"; break;
                                    case "3": sEffects[1] = "Pregnant"; break;
                                    case "4": sEffects[1] = "Pregnant By Player"; break;
                                    case "5": sEffects[1] = "Slave"; break;
                                    case "6": sEffects[1] = "Has Daughter"; break;
                                    case "7": sEffects[1] = "Has Son"; break;
                                    case "8": sEffects[1] = "Inseminated"; break;
                                    case "9": sEffects[1] = "Controlled"; break;
                                    case "10": sEffects[1] = "Catacombs"; break;
                                    case "11": sEffects[1] = "Arena"; break;
                                    case "12": sEffects[1] = "Your Daughter"; break;
                                    case "13": sEffects[1] = "Is Daughter"; break;
                                }
                                break;
                            case "4":
                                sEffects[0] = "Trait";
                                break;
                        }
                    }
                    effect.SetAttribute("What", sEffects[0]);
                    effect.SetAttribute("Name", sEffects[1]);
                    effect.SetAttribute("Amount", sEffects[2]);
                    if (sEffects[0] == "Trait" && tempitem)             // `J` only save duration for temporary traits.
                        effect.SetAttribute("Duration", sEffects[3]);

                    item.AppendChild(effect);
                }
                items.AppendChild(item);
            }
            xmldoc.AppendChild(items);

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.NewLineOnAttributes = false;
            settings.IndentChars = "    ";
            XmlWriter xmlwrite = XmlWriter.Create(path, settings);

            xmldoc.Save(xmlwrite);
            xmlwrite.Close();
        }

        //clears item list
        private void button_ItemsClearList_Click(object sender, EventArgs e)
        {
            ItemsCollection.Clear();
            listBox_ItemsList.Items.Clear();
            StatusLabel1.Text = "Item list cleared...";
        }

        //displays tooltips with items descriptions on items list
        private void listBox_ItemsList_MouseMove(object sender, MouseEventArgs e)
        {
            ListBox listBox = (ListBox)sender;								//creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
            int index = listBox.IndexFromPoint(e.Location);					//e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
            if (index > -1 && index < listBox.Items.Count)					//if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
            {
                string tip = listBox.Items[index].ToString();				//this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
                if (tip != lastTip)											//every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
                {															//also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
                    toolTip1.SetToolTip(listBox, new StringReader(ItemsCollection.Rows[index][1].ToString()).ReadLine());	//this is the only difference from previous tooltip function. Description is stored in dataTable at [index][1], along with other data, so to get it out we first need to identify coordinates (that's what [index][1] is for), then construct StringReader from that long string, and lastly read first line from that stream, this is description. Luckily it can be fitted in one line :P
                    lastTip = tip;											//updates lastTip string
                }
            }
        }


        //***************************
        //*****   Scripts tab   *****
        //***************************




        private void button_Script_Clear_Click(object sender, EventArgs e)
        {
            ScriptsCollection.Clear();
            label_Script.Text = "Work in progress.";
            label_Script_Path.Text = "Click Load Script to start.";

        }

        private void button_Script_Load_Click(object sender, EventArgs e)
        {
            OpenFileDialog Open = new OpenFileDialog();
            Open.Filter = "Whore Master Script File|*.scriptx; *.script|All files|*.*";
            Open.ShowDialog();
            button_Script_Clear_Click(sender, e);
            switch (Path.GetExtension(Open.FileName))
            {
                case ".script":
                    LoadScripts(Open.FileName);
                    break;
                case ".scriptx":
                    LoadScriptsXML(Open.FileName);
                    break;
            }
            //SortDataTable(ref ScriptsCollection, ref listBox_ScriptsList);
        }


        private void LoadScripts(string path)
        {
            try
            {
                Stream fp = File.Open(path, FileMode.Open);
                long Num = 0;
                BinaryReader read = new BinaryReader(fp);
                string ScriptName = path.Substring(path.LastIndexOf("\\") + 1, path.LastIndexOf(".") - path.LastIndexOf("\\") - 1);
                label_Script.Text = ScriptName;
                label_Script_Path.Text = path;
                Num = read.ReadInt32();
                for (int i = 0; i < Num; i++)
                {
                    int ActionNumber = i;
                    int a_Type = read.ReadInt32();
                    int a_NumEntries = read.ReadInt32();

                    ScriptsCollection.Rows.Add(ActionNumber, "", a_Type, a_NumEntries, "", "");
                    if (a_NumEntries>0)
                    {
                        for (int j = 0; j < a_NumEntries; j++)
                        {
                            string e_anumber = i.ToString();
                            string e_number = j.ToString();
                            string e_Type = read.ReadInt32().ToString();
                            int e_IOValue = read.ReadInt32();
                            char a = read.ReadChar();
                            int b = a;
                            string e_Var = b.ToString();
                            string e_Text = "";
                            if (e_Type=="1")
                            {
                                for (int k = 0; k < e_IOValue; k++)
                                {
                                    e_Text += read.ReadChar();
                                }
                            }
                            ScriptsCollection.Rows.Add(e_anumber, e_number, e_Type, e_IOValue.ToString(), e_Var, e_Text);
                        }
                    }
                }
                fp.Close();
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("File load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "Open error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "Load canceled...";
            }
        }

        private void LoadScriptsXML(string path)
        {
            try
            {
                XmlTextReader xmlread = new XmlTextReader(path);
                XmlDocument xmldoc = new XmlDocument();
                xmldoc.Load(xmlread);
                XmlNode baseNode = xmldoc.DocumentElement;
                
                string ScriptName="";
                string NumActions = "";
                if (baseNode.Attributes["ScriptName"] == null)
                    ScriptName = path.Substring(path.LastIndexOf("\\") + 1, path.LastIndexOf(".") - path.LastIndexOf("\\") - 1);
                else ScriptName = baseNode.Attributes["ScriptName"].Value;
                label_Script.Text = ScriptName;
                label_Script_Path.Text = path;

                if (baseNode.Attributes["NumActions"] == null) NumActions = "0";
                else NumActions = baseNode.Attributes["NumActions"].Value;

                foreach (XmlNode n_actions in baseNode.SelectNodes("/Root/Action"))
                {
                    string ActionNumber     = n_actions.Attributes["ActionNumber"].Value;
                    string a_Type           = n_actions.Attributes["Type"].Value;
                    string a_NumEntries     = n_actions.Attributes["NumEntries"].Value;

                    ScriptsCollection.Rows.Add(ActionNumber, "", a_Type, a_NumEntries, "", "");
                    if (n_actions.HasChildNodes == true)
                    {
                        foreach (XmlNode n_entry in n_actions.ChildNodes)
                        {
                            if (n_entry.NodeType != XmlNodeType.Element) continue;   // `J` only process elements

                            string e_anumber = ""; string e_number = ""; string e_Type = "";
                            string e_IOValue = ""; string e_Var = ""; string e_Text = "";
                            if (n_entry.Attributes["ActionNumber"] != null) e_anumber = n_entry.Attributes["ActionNumber"].Value;
                            if (n_entry.Attributes["EntryNumber"] != null)  e_number = n_entry.Attributes["EntryNumber"].Value;
                            if (n_entry.Attributes["Type"] != null)         e_Type = n_entry.Attributes["Type"].Value;
                            if (n_entry.Attributes["IOValue"] != null)      e_IOValue = n_entry.Attributes["IOValue"].Value;
                            if (n_entry.Attributes["Var"] != null)          e_Var = n_entry.Attributes["Var"].Value;
                            if (n_entry.Attributes["Text"] != null)         e_Text = n_entry.Attributes["Text"].Value;
                            ScriptsCollection.Rows.Add(e_anumber, e_number, e_Type, e_IOValue, e_Var, e_Text);
                        }
                    }
                }
                //ScriptsCollection.Rows.Add(sName, sDesc + "\r\n" + sData + "\r\n" + num.ToString() + "\r\n" + sEffects);
                xmlread.Close();
                StatusLabel1.Text = "Loaded Script file: '" + ScriptName+ "'.";
                //StatusLabel1.Text = "Loaded " + ScriptName + " from XML file...";
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(path, err.Message);
                MessageBox.Show("File load error in file \n\n" + path + "\n\n" + err.Message + "\n\nSee the Extras tab for details.", "XML load error error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                StatusLabel1.Text = "XML load error...";
            }
        }

        private void button_Script_Save_Click(object sender, EventArgs e)
        {
            saveScript.Filter = "Whore Master XML Script file|*.scriptx|Whore Master Script file|*.script|All files|*.*";
            saveScript.DefaultExt = ".script";
            if (sScriptPath != "")
            {
                saveScript.FileName = Path.GetFileName(sScriptPath);
                saveScript.InitialDirectory = Path.GetDirectoryName(sScriptPath);
            }
            try
            {
                saveScript.ShowDialog();
                if (File.Exists(Convert.ToString(saveScript.FileName) + ".bak") == true) File.Delete(Convert.ToString(saveScript.FileName) + ".bak");
                if (File.Exists(Convert.ToString(saveScript.FileName)) == true) File.Move(Convert.ToString(saveScript.FileName), Convert.ToString(saveScript.FileName) + ".bak");
                switch (Path.GetExtension(saveScript.FileName))
                {
                    case ".script":
                        saveScriptBinary(saveScript.FileName);
                        StatusLabel1.Text = "Script saved...";
                        break;
                    case ".scriptx":
                    default:
                        saveScriptXML(saveScript.FileName);
                        StatusLabel1.Text = "Script XML saved...";
                        break;
                }
            }
            catch (Exception err)
            {
                dataGridView_Bad_Files.Rows.Add(saveScript.FileName, err.Message);
                StatusLabel1.Text = "XML Script save error...";
            }
        }

        private void saveScriptBinary(string path)
        {
// gamecode //              bool SaveScriptFile(const char *Filename, sScript *ScriptRoot)
// gamecode //  {
// gamecode //  	FILE *fp;
// gamecode //  	long i, j, NumActions;
// gamecode //  	sScript *ScriptPtr;
// gamecode //  
// gamecode //  	// Make sure there are some script actions
// gamecode //  	if((ScriptPtr = ScriptRoot) == 0)
// gamecode //  		return false;
// gamecode //  
// gamecode //  	// Count the number of actions
// gamecode //  	NumActions = 0;
// gamecode //  	while(ScriptPtr != 0) 
// gamecode //  	{
// gamecode //  		NumActions++; // Increase count
// gamecode //  		ScriptPtr = ScriptPtr->m_Next; // Next action
// gamecode //  	}
// gamecode //  
// gamecode //  	// Open the file for output
// gamecode //  	if ((fp = fopen(Filename, "wb")) == 0) return false; // return a failure
// gamecode //  
// gamecode //  	// Output # of script actions
// gamecode //  	fwrite(&NumActions, 1, sizeof(long), fp);
// gamecode //  
// gamecode //  	// Loop through each script action
// gamecode //  	ScriptPtr = ScriptRoot;
// gamecode //  	for(i=0;i<NumActions;i++) 
// gamecode //  	{
// gamecode //  		// Output type of action and # of entries
// gamecode //  		fwrite(&ScriptPtr->m_Type, 1, sizeof(long), fp);
// gamecode //  		fwrite(&ScriptPtr->m_NumEntries, 1, sizeof(long), fp);
// gamecode //  
// gamecode //  		// Output entry data (if any)
// gamecode //  		if(ScriptPtr->m_NumEntries) 
// gamecode //  		{
// gamecode //  			for(j=0;j<ScriptPtr->m_NumEntries;j++) 
// gamecode //  			{
// gamecode //  				// Write entry type and data
// gamecode //  				fwrite(&ScriptPtr->m_Entries[j].m_Type, 1,sizeof(long),fp);
// gamecode //  				fwrite(&ScriptPtr->m_Entries[j].m_IOValue,1,sizeof(long),fp);
// gamecode //  				fwrite(&ScriptPtr->m_Entries[j].m_Var,1,sizeof(unsigned char),fp);
// gamecode //  
// gamecode //  				// Write text entry (if any)
// gamecode //  				if(ScriptPtr->m_Entries[j].m_Type == _TEXT && ScriptPtr->m_Entries[j].m_Text != NULL)
// gamecode //  					fwrite(ScriptPtr->m_Entries[j].m_Text, 1, ScriptPtr->m_Entries[j].m_Length, fp);
// gamecode //  			}
// gamecode //  		}
// gamecode //  	
// gamecode //  		// Go to next script structure in linked list
// gamecode //  		ScriptPtr = ScriptPtr->m_Next;
// gamecode //  	}
// gamecode //  
// gamecode //  	fclose(fp);
// gamecode //  	return true; // return a success!
// gamecode //  }


        }
        private void saveScriptXML(string path)
        {
            SortScript(ScriptsCollection);

            XmlDocument xmldoc = new XmlDocument();

            XmlElement sRoot = xmldoc.CreateElement("Root");
            XmlElement sAction = xmldoc.CreateElement("Action");
            XmlElement sEntry = xmldoc.CreateElement("Entry");

            string cAction = "";
            for (int x = 0; x < ScriptsCollection.Rows.Count; x++)
            {
                string a = ScriptsCollection.Rows[x]["Script_ActionNumber"].ToString();
                string b = ScriptsCollection.Rows[x]["Script_EntryNumber"].ToString();
                string c = ScriptsCollection.Rows[x]["Script_Type"].ToString();
                string d = ScriptsCollection.Rows[x]["Script_IOValue"].ToString();
                string e = ScriptsCollection.Rows[x]["Script_Var"].ToString();
                string f = ScriptsCollection.Rows[x]["Script_Text"].ToString();


                if (cAction != a)
                {
//<Action ActionNumber="2" Type="6" NumEntries="1">
                    if (cAction!="") sRoot.AppendChild(sAction);
                    cAction = a;
                    sAction = xmldoc.CreateElement("Action");
                    sAction.SetAttribute("ActionNumber", a);
                    sAction.SetAttribute("Type", c);
                    sAction.SetAttribute("NumEntries", d);
                }
                else
                {
//<Entry ActionNumber="2" EntryNumber="0" Type="1" IOValue="7" Var="0" Text="Reward" />
                    sEntry = xmldoc.CreateElement("Entry");
                    sEntry.SetAttribute("ActionNumber", a);
                    sEntry.SetAttribute("EntryNumber", b);
                    sEntry.SetAttribute("Type", c);
                    sEntry.SetAttribute("IOValue", d);
                    sEntry.SetAttribute("Var", e);
                    if (f.Length > 0)
                    {
                        f = f.Substring(0, f.LastIndexOf('\0'));
                        sEntry.SetAttribute("Text", f);
                    }
                    sAction.AppendChild(sEntry);
                }
            }
            sRoot.AppendChild(sAction);
            sRoot.SetAttribute("NumActions", cAction);
            xmldoc.AppendChild(sRoot);

            XmlWriterSettings settings = new XmlWriterSettings();
            settings.Indent = true;
            settings.NewLineOnAttributes = false;
            settings.IndentChars = "    ";
            XmlWriter xmlwrite = XmlWriter.Create(path, settings);

            xmldoc.Save(xmlwrite);
            xmlwrite.Close();

        }

        private void SortScript(DataTable dt)
        {
            DataTable tempDT = dt.Clone();							//copies the structure from original DataTable

            for (int x = 0; x < dt.Rows.Count; x++) tempDT.ImportRow(dt.Rows[x]);	//this copies content from original DataTable
            DataView v = tempDT.DefaultView;							//rest is almost the same as with normal sort, create DataView from our DataTable, this time temp DataTable that will get gutted :P, so our original doesn't loose any data
            v.Sort = "Script_ActionNumber ASC, Script_EntryNumber ASC";										//sort this DataView in ascending order using "Name" column as key
            dt = v.ToTable();									//apply this sorted view to our original DataTable

        }



        //*********************************
        //*****   General functions   *****
        //*********************************

        //complicated way to sort DataTable :P, currently unused, but if need be it can stay here
        private static void SortDataTable_old(DataTable dt, string sort)
        {
            DataTable newDT = dt.Clone();
            int rowCount = dt.Rows.Count;

            DataRow[] foundRows = dt.Select(null, sort);// Sort with Column name
            for (int i = 0; i < rowCount; i++)
            {
                object[] arr = new object[dt.Columns.Count];
                for (int j = 0; j < dt.Columns.Count; j++)
                {
                    arr[j] = foundRows[i][j];
                }
                DataRow data_row = newDT.NewRow();
                data_row.ItemArray = arr;
                newDT.Rows.Add(data_row);
            }

            //clear the incoming dt
            dt.Rows.Clear();

            for (int i = 0; i < newDT.Rows.Count; i++)
            {
                object[] arr = new object[dt.Columns.Count];
                for (int j = 0; j < dt.Columns.Count; j++)
                {
                    arr[j] = newDT.Rows[i][j];
                }

                DataRow data_row = dt.NewRow();
                data_row.ItemArray = arr;
                dt.Rows.Add(data_row);
            }

        }

        //sorts dataTables
        private void SortDataTable(ref DataTable dt, ref ListBox lb)
        {
            DataView v = dt.DefaultView;							//create DataView from our DataTable
            v.Sort = "Name ASC";									//sort this DataView in ascending order using "Name" column as key
            dt = v.ToTable();									//apply this sorted view to our original DataTable

            lb.Items.Clear();									//empty listbBox from entries it has

            for (int x = 0; x < dt.Rows.Count; x++)				//go through all records in DataTable and add names to listBox so our index sync works again
            {
                lb.Items.Add(dt.Rows[x][0].ToString());
            }
        }

        //same as above, but selects an entry after it's done (used for add to list, and update on list buttons)
        private void SortDataTable(ref DataTable dt, ref ListBox lb, string Name)
        {
            DataView v = dt.DefaultView;
            v.Sort = "Name ASC";
            dt = v.ToTable();
            lb.Items.Clear();
            for (int x = 0; x < dt.Rows.Count; x++)
            {
                lb.Items.Add(dt.Rows[x][0].ToString());
            }
            lb.SelectedItem = Name;
        }

        private void SortDataTable(ref DataTable dt, ref ListBox lb, string Column, bool ASC0DESC1)
        {
            DataView v = dt.DefaultView;							//create DataView from our DataTable
            v.Sort = Column + " " + (ASC0DESC1 ? "DESC" : "ASC");   //sort this DataView in ascending order using "Name" column as key
            
            dt = v.ToTable();									//apply this sorted view to our original DataTable

            lb.Items.Clear();									//empty listbBox from entries it has

            for (int x = 0; x < dt.Rows.Count; x++)				//go through all records in DataTable and add names to listBox so our index sync works again
            {
                lb.Items.Add(dt.Rows[x][0].ToString());
            }
        }

        //event to prevent other stuff to be dropped, it accepts only files
        private void App_DragEnter(object sender, DragEventArgs e)
        {
            // just to be sure they're actually dropping files (not text or anything else)
            if (e.Data.GetDataPresent(DataFormats.FileDrop, false) == true)
                // allow them to continue
                // (without this, the cursor stays a "NO" symbol
                e.Effect = DragDropEffects.All;
        }

        //what happens when you drop files, it simply checks extension of each file dropped (if there are more than one) and send it to it's load function accordingly
        private void App_FileDrop(object sender, DragEventArgs e)
        {
            // transfer the filenames to a string array
            // (yes, everything to the left of the "=" can be put in the 
            // foreach loop in place of "files", but this is easier to understand.)
            string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);

            // loop through the string array, processing each filename
            foreach (string file in files)
            {
                switch (Path.GetExtension(file))
                {
                    case ".girls":
                        LoadGirls(file);
                        tabControl1.SelectedTab = tabPage1_Girls;
                        break;
                    case ".girlsx":
                        LoadGirlsXML(file);
                        tabControl1.SelectedTab = tabPage1_Girls;
                        break;
                    case ".rgirls":
                        LoadRGirls(file);
                        tabControl1.SelectedTab = tabPage2_RGirls;
                        break;
                    case ".rgirlsx":
                        LoadRGirlsXML(file);
                        tabControl1.SelectedTab = tabPage2_RGirls;
                        break;
                    case ".items":
                        LoadItems(file);
                        tabControl1.SelectedTab = tabPage3_Items;
                        break;
                    case ".itemsx":
                        LoadItemsXML(file);
                        tabControl1.SelectedTab = tabPage3_Items;
                        break;
                }

            }
        }

        string G_Create_Real_Name(string FN, string MN, string LN)
        {
            string name;
            int name1 = 0; int name2 = 0; int name3 = 0; int names = 0;
            if (FN.Length > 0) { names++; name1++; }
            if (MN.Length > 0) { names++; name2++; }
            if (LN.Length > 0) { names++; name3++; }

            if (names == 3)         name = FN + " " + MN + " " + LN;
            else if (names == 2)
            {
                if (name1 == 1)     name = FN + " " + MN + LN;
                else                name = MN + " " + LN;
            }
            else                    name = FN + MN + LN;
            return name;
        }
        string[] G_Divide_Real_Name(string name)
        {
            string[] ret = new string[3];
            ret[0] = ""; ret[1] = ""; ret[2] = "";
            string[] splitname = name.Split(' ');
            string rename = "";
            // first we remove any extra spaces
            for (int i = 0; i < splitname.Length; i++)
            {
                if (splitname[i].Length > 0)
                {
                    if (rename.Length > 0) rename += " ";
                    rename += splitname[i];
                }
            }
            splitname = rename.Split(' ');
            if (splitname.Length < 1) return ret;

            ret[0] = splitname[0];

            if (splitname.Length == 2) ret[2] = splitname[1];
            else if (splitname.Length > 2)
            {
                for (int i = 1; i < splitname.Length - 1; i++)
                {
                    if (ret[1].Length > 0) ret[1] += " ";
                    ret[1] += splitname[i];
                }
                ret[2] = splitname[splitname.Length - 1];
            }
            return ret;
        }

        private void button_G_Create_Real_Name_Click(object sender, EventArgs e)
        {
            TBox_G_RealName.Text = G_Create_Real_Name(TBox_G_FirstName.Text, TBox_G_MiddleName.Text, TBox_G_SurName.Text);
        }

        private void button_G_Divide_Real_Name_Click(object sender, EventArgs e)
        {
            string[] names = new string[3];
            names = G_Divide_Real_Name(TBox_G_RealName.Text);
            TBox_G_FirstName.Text = names[0];
            TBox_G_MiddleName.Text = names[1];
            TBox_G_SurName.Text = names[2];

    
            /*
             * `J` end of file 
             * Anything after this line was added by the editing program
             * and can probably be removed
             */
        }


   }
}