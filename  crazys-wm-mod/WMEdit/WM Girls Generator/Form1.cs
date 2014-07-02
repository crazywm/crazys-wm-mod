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
		ArrayList aTraits = new ArrayList();						//ArrayList to store trait info (experiment for trait tooltips), apparently it worked
		DataTable iTable = new DataTable();						 //DataTable for items effects
		DataTable iTTable = new DataTable();						//DataTable for item traits
		DataTable ItemsCollection = new DataTable();				//DataTable to store items in
		DataTable GirlsCollection = new DataTable();				//DataTable to store girls in
		DataTable GirlsCollectionTemp = new DataTable();			//DataTable to store temporary filtered girls in
		DataTable RGirlsCollection = new DataTable();			   //DataTable to store random girls in
		DataTable rgTable = new DataTable();						//table to store selected traits and chances for random girls
		SaveFileDialog Filesave = new SaveFileDialog();
		string lastTip = "";										//temp storage string for specific item in listbox' to function
		string sConfigPath = "";
		Random rnd = new Random();								  //creates new random object to generate random values where needed


		public Form1()
		{
			InitializeComponent();
			comboBox_GirlType.SelectedIndex = 0;					//sets default selections for these comboBoxes
			comboBox_ItemType_01.SelectedIndex = 3;
			comboBox_Rarity_05.SelectedIndex = 0;
			comboBox_ItemDuration.SelectedIndex = 0;
			checkBox_ToggleTraitTooltips.Visible = false;		   //hides traits checkbox by default
			StatusLabel1.Text = "";								 //clears text in status bar

			
			string sTraitsPath = "Resources\\Data\\CoreTraits.traits";	//Default path to CoreTraits.traits file
			string sConfigPath = "Resources\\Data\\config.xml";				 //Default path to config.xml


			if (File.Exists(sTraitsPath))								 //Checks to determine editor executable is in one of predefined places so it can load CoreTraits.traits file automaticaly
			{
				LoadTraits(sTraitsPath);								  //if check passes (file is found at this path) calls this function and sends this path to it, it's defined below
				tabControl1.SelectedTab = tabPage1;					 //if it finds it by itself it switches to Girls tab automatically, if neither of this combinations work it will default to Info tab
				checkBox_ToggleTraitTooltips.Visible = true;
			}
			else if (File.Exists("..\\" + sTraitsPath))
			{
				LoadTraits("..\\" + sTraitsPath);
				tabControl1.SelectedTab = tabPage1;
				checkBox_ToggleTraitTooltips.Visible = true;
			}
			else if (File.Exists("..\\..\\" + sTraitsPath))
			{
				LoadTraits("..\\..\\" + sTraitsPath);
				tabControl1.SelectedTab = tabPage1;
				checkBox_ToggleTraitTooltips.Visible = true;
			}

			if (File.Exists(sConfigPath))								   //checks where config.xml is
			{
				LoadConfigXML(sConfigPath);
			}
			else if (File.Exists("..\\" + sConfigPath))
			{
				LoadConfigXML("..\\" + sConfigPath);
			}
			else if (File.Exists("..\\..\\" + sConfigPath))
			{
				LoadConfigXML("..\\..\\" + sConfigPath);
			}
			

			rgTable.Columns.Add("Trait", typeof(string));			   //adds columns to rgTable dataTable
			rgTable.Columns.Add("Chance", typeof(int));
			dataGridView1.DataSource = rgTable;						 //links datagrid on form with this dataTable, below are some options to set behaviour of this datagrid
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

			iTTable.Columns.Add("Traits", typeof(string));				  //Adding columns to items traits table
			iTTable.Columns.Add("Value", typeof(string));
			dataGridView3.DataSource = iTTable;
			dataGridView3.RowHeadersVisible = false;
			dataGridView3.AllowUserToAddRows = false;
			dataGridView3.Columns[0].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
			dataGridView3.Columns[0].ReadOnly = true;
			dataGridView3.Columns[1].AutoSizeMode = DataGridViewAutoSizeColumnMode.Fill;
			dataGridView3.Columns[1].ReadOnly = true;

			ItemsCollection.Columns.Add("Name", typeof(string));			//formatting of items DataTable, adding two columns, Name and Data
			ItemsCollection.Columns.Add("Data", typeof(string));

			GirlsCollection.Columns.Add("Name", typeof(string));			//formatting of girls DataTable, adding two columns, Name and Data
			GirlsCollection.Columns.Add("Data", typeof(string));
			GirlsCollection.Columns.Add("TypeFilter", typeof(string));	  //this was added at later point, in this column goes girl type (slave, normal...) so they could be easily filtered sorted by that key

			RGirlsCollection.Columns.Add("Name", typeof(string));		   //formatting of random girls DataTable, adding two columns, Name and Data
			RGirlsCollection.Columns.Add("Data", typeof(string));

			this.comboBox_SortByType.SelectedIndexChanged -= new System.EventHandler(this.comboBox_SortByType_SelectedIndexChanged);
			comboBox_SortByType.SelectedIndex = 0;				  //sets girl type droplist to show "All" as default
			this.comboBox_SortByType.SelectedIndexChanged += new System.EventHandler(this.comboBox_SortByType_SelectedIndexChanged);
		}

		//Function that populates traits checkboxes and traits tab
		private void LoadTraits(string path)
		{
			try
			{
				StreamReader Import = new StreamReader(path);			   //opens StreamReader and points it to path recieved when function was called
				while (Import.Peek() >= 0)								  //basically do while EoF
				{
					string temp1 = Convert.ToString(Import.ReadLine());	 //reads first line from traits file (this is trait name), it could be added directly to trait trait list, but this way I can use it multiple times (to fill traits tab and trait tooltips)
					string temp2 = Convert.ToString(Import.ReadLine());	 //second line this is trait description
					checkedListBox1.Items.Add(temp1);					   //adds trait to checkedListBox for girls
					comboBox_ItemTraits.Items.Add(temp1);				   //fills droplist on item tab with traits
					comboBox_RGTraits.Items.Add(temp1);					 //same with droplist on random girls tab

					traitsTBox1.Text = traitsTBox1.Text + temp1 + " - " + temp2 + "\r\n\r\n";	   //this one populates traits tab with "trait - trait description" format
					aTraits.Add(temp2);									 //fills aTraits ArrayList with traits description, they are called as traits tooltips description, each trait in listbox has the same index as description here so it's easy to link them
				}
				Import.Close();
				StatusLabel1.Text = "Loaded traits file...";				//updates status line with this message
				comboBox_RGTraits.SelectedIndex = 0;						//after filling random girls droplist it sets it to first entry
			}
			catch (Exception err)
			{
				MessageBox.Show(err.Message, "Open trait file error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}

		}

		//hides or shows checkBox for traits list on girls tab in relationship with currently selectd tab
		private void tabControl1_Selected(object sender, TabControlEventArgs e)
		{
			if (tabControl1.SelectedTab == tabPage1) checkBox_ToggleTraitTooltips.Visible = true;
			else checkBox_ToggleTraitTooltips.Visible = false;
		}

		//*************************
		//*****   Info tab   *****
		//*************************

		//Load traits button on info page, if it couldn't find it by itself user will have to find it manually, basically it calls LoadTraits function with path to the file user browsed to
		private void button2_Click(object sender, EventArgs e)
		{
			OpenFileDialog openTraits = new OpenFileDialog();

			openTraits.Filter = "Text Document|*.traits|All Files|*.*";
			//try
			//{
				openTraits.ShowDialog();
				LoadTraits(openTraits.FileName);
				tabControl1.SelectedTab = tabPage1;
			//}
			//catch (Exception err)
			//{
			//	MessageBox.Show("You have to Select CoreTraits.traits file to use for traits database!\n\rOr if you did and you still got this message there could be a problem with file permissions.\n\rNext is the \"official\" error:\n\r\n\r" + err.Message, "Traits file error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			//}
		}

		//Load config.xml file
		private void button_Info_ConfigLoad_Click(object sender, EventArgs e)
		{
			OpenFileDialog openConfig = new OpenFileDialog();
			openConfig.Filter = "Whore Master XML config file|*.xml|All files|*.*";
			openConfig.InitialDirectory = Application.StartupPath;
			try
			{
				openConfig.ShowDialog();
				LoadConfigXML(openConfig.FileName);
			}
			catch
			{

			}

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

		//*************************
		//*****   Girls tab   *****
		//*************************

		//Add girl to girls list
		private void button1_Click(object sender, EventArgs e)
		{
			if (comboBox_SortByType.SelectedItem.ToString() == "All")
			{
				GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));   //adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
				SortDataTable(ref GirlsCollection, ref listBox_GirlsList, NameTBox_01.Text);		//Resorts the DataTable with girls so list will be in alphabetical order
				StatusLabel1.Text = "Added girl " + "\"" + GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][0] + "\"" + " to list...";	 //changes status line text to this
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Normal girls")
			{
				GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));   //adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "0", NameTBox_01.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
				StatusLabel1.Text = "Added girl " + "\"" + NameTBox_01.Text + "\"" + " to list...";	 //changes status line text to this
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Slave girls")
			{
				GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));   //adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "1", NameTBox_01.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
				StatusLabel1.Text = "Added girl " + "\"" + NameTBox_01.Text + "\"" + " to list...";	 //changes status line text to this
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Catacombs girls")
			{
				GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));   //adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "2", NameTBox_01.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
				StatusLabel1.Text = "Added girl " + "\"" + NameTBox_01.Text + "\"" + " to list...";	 //changes status line text to this
			}
            else if (comboBox_SortByType.SelectedItem.ToString() == "Arena girls")
            {
                GirlsCollection.Rows.Add(CompileGirl().ElementAt(0), CompileGirl().ElementAt(1), Convert.ToInt32(CompileGirl().ElementAt(2)));   //adds girl entry to Girls DataTable via CompileGirl function, that function returns array of strings, .ElementAt accesses one of these array elements, first element could have been copied directly from name textbox but...
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "3", NameTBox_01.Text);		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Added girl " + "\"" + NameTBox_01.Text + "\"" + " to list...";	 //changes status line text to this
            }
        }

		//Overwrites/updates selected girl in girl's list
		private void button9_Click(object sender, EventArgs e)
		{
			if (comboBox_SortByType.SelectedItem.ToString() == "All")
			{
				GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][0] = CompileGirl().ElementAt(0);	  //overwrite selected cell in dataTable with new name
				GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][1] = CompileGirl().ElementAt(1);	  //overwrite selected cell in dataTable with new data
				GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][2] = CompileGirl().ElementAt(2);	  //overwrite selected cell in dataTable with new data
				SortDataTable(ref GirlsCollection, ref listBox_GirlsList, NameTBox_01.Text);				//Resorts the DataTable with girls so list will be in alphabetical order
				StatusLabel1.Text = "Updated item entry No" + (listBox_GirlsList.SelectedIndex + 1).ToString() + " " + "(\"" + GirlsCollection.Rows[listBox_GirlsList.SelectedIndex][0] + "\")...";
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Normal girls")
			{
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	  //overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	  //that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	  //in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "0");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
				StatusLabel1.Text = "Updated item entry " + "(\"" + NameTBox_01.Text + "\")...";
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Slave girls")
			{
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	  //overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	  //that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	  //in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "1");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
				StatusLabel1.Text = "Updated item entry " + "(\"" + NameTBox_01.Text + "\")...";
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Catacombs girls")
			{
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	  //overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	  //that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
				GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	  //in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "2");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
				StatusLabel1.Text = "Updated item entry " + "(\"" + NameTBox_01.Text + "\")...";
			}
            else if (comboBox_SortByType.SelectedItem.ToString() == "Arena girls")
            {
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][0] = CompileGirl().ElementAt(0);	  //overwrite selected cell in dataTable with new name, this is where that ID column in temp DataTable comes to use, we use it to know what row in original DT needs to be updated
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][1] = CompileGirl().ElementAt(1);	  //that's the "GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]" part
                GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]][2] = CompileGirl().ElementAt(2);	  //in filter case listBox is in sync with temp DT so we use index of listBox to access coresponding row in temp DT, ID is fourth column, this corresponds to 3 since counting goes from 0
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "3");		//since filtering is already selected we simply run it again to filter main list again and build filtered list
                StatusLabel1.Text = "Updated item entry " + "(\"" + NameTBox_01.Text + "\")...";
            }

		}

		//compile girl method, I moved it from button(s) code so it's easier to manage, I just need to take care of this one and changes will affect both buttons
		private string[] CompileGirl()
		{
			string[] sGirl = { "", "", "" };	   //string in which girl data will be compiled
			string nln = "\r\n";			//to make it little easier to write I define this as separate string so when I need to go to next line i just enter nln in string
			string sp = " ";				//same with space
			string sTraits = "";			//string to store selected traits in
			string sStats = "";			 //string for stats
			string sSkills = "";			//for skills
			string sSlave = "";			 //ex is slave/isn't slave option, now has one more state
			string sGDesc = "";			 //girl description


			int i = 0;
			while (i < checkedListBox1.CheckedItems.Count)	  //this while loop goes through selected traits and puts them in sTraits string
			{
				sTraits = sTraits + checkedListBox1.CheckedItems[i].ToString() + nln;		   //since every trait is in separate line I need to add carraige return at the end, this is where that "nln" comes in
				i = i + 1;
			}

			sGDesc = DescTBox_01.Text;				  //originally description was pulled directly from desc textbox, but game has some issues if description is empty so I put these two lines, first picks up what's in desc textbox, and second checks if it's emtpy, if it is then it puts - in there so it won't be empty
			if (sGDesc.Length == 0) sGDesc = "-";

			//next two lines are collection of numbers from stats and skills textboxes,and this is where that "sp" comes in handy, it's easier to type sp than " " every time
			sStats = StatsTBox_01.Text + sp + StatsTBox_02.Text + sp + StatsTBox_03.Text + sp + StatsTBox_04.Text + sp + StatsTBox_05.Text + sp + StatsTBox_06.Text + sp + StatsTBox_07.Text + sp + StatsTBox_08.Text + sp + StatsTBox_09.Text + sp + StatsTBox_10.Text + sp + StatsTBox_11.Text + sp + StatsTBox_12.Text + sp + StatsTBox_13.Text + sp + StatsTBox_14.Text + sp + StatsTBox_15.Text + sp + StatsTBox_16.Text + sp + StatsTBox_17.Text + sp + StatsTBox_18.Text + sp + StatsTBox_19.Text + sp + StatsTBox_20.Text + sp + StatsTBox_21.Text + sp + StatsTBox_22.Text;
			sSkills = SkillTBox_01.Text + sp + SkillTBox_02.Text + sp + SkillTBox_03.Text + sp + SkillTBox_04.Text + sp + SkillTBox_05.Text + sp + SkillTBox_06.Text + sp + SkillTBox_07.Text + sp + SkillTBox_08.Text + sp + SkillTBox_09.Text + sp + SkillTBox_10.Text + sp + SkillTBox_11.Text + sp + SkillTBox_12.Text + sp + SkillTBox_13.Text + sp + SkillTBox_14.Text + sp + SkillTBox_15.Text; 
			if (comboBox_GirlType.SelectedIndex == 1) sSlave = "1";		 //if check to see if slave checkbox is checked or not, and to set slave flag accordingly
			else if (comboBox_GirlType.SelectedIndex == 2) sSlave = "2";	//maybe I should have simply said "sSlave = comboBox_GirlType.SelectedIndex.ToString();" that would eliminate ifs, but this way it's more obvious, and it's not necessary to have these items in specific order in dropbox (although they are at the moment)
            else if (comboBox_GirlType.SelectedIndex == 3) sSlave = "3";
            else sSlave = "0";

			sGirl[0] = NameTBox_01.Text;				//first element in array is name
			sGirl[1] = sGDesc + nln + checkedListBox1.CheckedItems.Count + nln + sTraits + "0" + nln + sStats + nln + sSkills + nln + GoldTBox1.Text + nln + sSlave;	  //second is girl data, this is where nln comes handy, easier to type nln than "\n\r" every time, and less error prone
			sGirl[2] = sSlave;

			return sGirl;
		}

		//Randomize stats button
		private void button3_Click(object sender, EventArgs e)
		{
			StatsTBox_01.Text = rnd.Next(1, 101).ToString();			//Charisma
			StatsTBox_03.Text = rnd.Next(1, 101).ToString();			//Libido
			StatsTBox_04.Text = rnd.Next(1, 101).ToString();			//Constitution
			StatsTBox_05.Text = rnd.Next(1, 101).ToString();			//Intelligence
			StatsTBox_06.Text = rnd.Next(1, 101).ToString();			//Confidence
			StatsTBox_07.Text = rnd.Next(1, 101).ToString();			//Mana
			StatsTBox_08.Text = rnd.Next(1, 101).ToString();			//Agility
			StatsTBox_14.Text = rnd.Next(18, 101).ToString();		   //Age
			StatsTBox_15.Text = rnd.Next(1, 101).ToString();			//Obedience
			StatsTBox_16.Text = rnd.Next(1, 101).ToString();			//Spirit
			StatsTBox_17.Text = rnd.Next(1, 101).ToString();			//Beauty		  
		}

		//Conditional randomize stats button, this will have way more fiddling than normal randomize to limit results as to my observances of what these usually are
		private void button4_Click(object sender, EventArgs e)
		{
			//Charisma stat, I'm changing the way it's generated after i come up with it few code lines below, if you want to know what it does check intellignce description below
			StatsTBox_01.Text = (rnd.Next(2, 5) * 10).ToString();

			StatsTBox_03.Text = (rnd.Next(1, 6) * 10).ToString();			 //Libido

			//Constitution stat, this garble below is cheap way how to assign it value 40, 50, 60 or 70, here is what it does. First IF checks if any trait
			//is selected. If not stat is set to 40 or 50, depending on random chance (rnd thingy in nested IF), if there are some checked it goes through them and looks if adventurer, 
			//assassin or cool scars trait is selected, if it is(second IF), it has 50:50 chance constitution will be 60 or 70, that's what rnd thingy in third IF does
			//it gives random number between 2 and 9, upper value excluded, so result can be between 2 and 8 (I tried first with only 2 numbers, but it didn't look that random, 
			//it mostly stayed in one value with occasional jump to other, if it's 4 or less it sets constitution to 60, other to 70.
			//One occurance of any of these traits is enough to justify high constitution so to stop going through rest of checked traits it breaks from WHILE
			//Lastly, if neither of these three traits is selected ELSE comes in effect and again 50:50 chance for constitution to be 40 or 50
			//There, I hope this is last rewrite of this explanation. I rewrote it at least 3 times because I thought of another way to do some stuff
			if (checkedListBox1.CheckedItems.Count == 0) if (rnd.Next(2, 9) >= 4) StatsTBox_04.Text = "40";
				else StatsTBox_04.Text = "50";
			else
			{
				int i = 0;
				while (i < checkedListBox1.CheckedItems.Count)
				{
					if (checkedListBox1.CheckedItems[i].ToString() == "Adventurer" || checkedListBox1.CheckedItems[i].ToString() == "Assassin" || checkedListBox1.CheckedItems[i].ToString() == "Cool Scars")
					{
						if (rnd.Next(2, 9) >= 4) StatsTBox_04.Text = "60";
						else StatsTBox_04.Text = "70";
						break;
					}
					else if (rnd.Next(2, 9) >= 4) StatsTBox_04.Text = "40";
					else StatsTBox_04.Text = "50";
					i = i + 1;
				}
			}

			//Intelligence stat, another mess to set it to 10, 20, 30, 40... again, why would it be simple if it can get complicated :P
			//I first wanted to make one random number from 1 to 10 and then with bunch of if checks give it 10 for 1, 20 for 2 and so on
			//But when I began typing I thought why? I don't need a check, I can simply multiply it by 10 and assign to stat.
			//Now to implement this for charisma and libido :P
			StatsTBox_05.Text = (rnd.Next(1, 11) * 10).ToString();

			//Confidence, by now you should know what I did here
			StatsTBox_06.Text = (rnd.Next(2, 9) * 10).ToString();

			//Mana, and also here
			if (rnd.Next(2, 9) >= 4) StatsTBox_07.Text = "0";
			else StatsTBox_07.Text = "20";

			//Agility
			StatsTBox_08.Text = (rnd.Next(3, 10) * 10).ToString();

			//Age, I'll limit it from 18 to 70
			StatsTBox_14.Text = rnd.Next(18, 71).ToString();

			//Obedience
			StatsTBox_15.Text = (rnd.Next(2, 7) * 10).ToString();

			//Spirit
			StatsTBox_16.Text = (rnd.Next(3, 7) * 10).ToString();

			//Beauty
			StatsTBox_17.Text = (rnd.Next(5, 7) * 10).ToString();
		}

		//Normalized randomize stats button, this one gives values rounded to 10, except for age
		private void button5_Click(object sender, EventArgs e)
		{
			StatsTBox_01.Text = (rnd.Next(1, 11) * 10).ToString();			//Charisma
			StatsTBox_03.Text = (rnd.Next(1, 11) * 10).ToString();		   //Libido
			StatsTBox_04.Text = (rnd.Next(1, 11) * 10).ToString();			//Constitution
			StatsTBox_05.Text = (rnd.Next(1, 11) * 10).ToString();		   //Intelligence
			StatsTBox_06.Text = (rnd.Next(1, 11) * 10).ToString();			//Confidence
			StatsTBox_07.Text = (rnd.Next(1, 11) * 10).ToString();			//Mana
			StatsTBox_08.Text = (rnd.Next(1, 11) * 10).ToString();			//Agility
			StatsTBox_14.Text = rnd.Next(18, 101).ToString();				 //Age
			StatsTBox_15.Text = (rnd.Next(1, 11) * 10).ToString();			//Obedience
			StatsTBox_16.Text = (rnd.Next(1, 11) * 10).ToString();			//Spirit
			StatsTBox_17.Text = (rnd.Next(1, 11) * 10).ToString();			//Beauty
		}

		//Randomize skills button
		private void button6_Click(object sender, EventArgs e)
		{
			SkillTBox_01.Text = rnd.Next(0, 101).ToString();		//Anal Sex
			SkillTBox_02.Text = rnd.Next(0, 101).ToString();		//Magic Ability
			SkillTBox_03.Text = rnd.Next(0, 101).ToString();		//BDSM Sex
			SkillTBox_04.Text = rnd.Next(0, 101).ToString();		//Normal Sex
			SkillTBox_05.Text = rnd.Next(0, 101).ToString();		//Bestiality Sex
			SkillTBox_06.Text = rnd.Next(0, 101).ToString();		//Group Sex
			SkillTBox_07.Text = rnd.Next(0, 101).ToString();		//Lesbian Sex
			SkillTBox_08.Text = rnd.Next(0, 101).ToString();		//Service Skills
			SkillTBox_09.Text = rnd.Next(0, 101).ToString();		//Stripping Sex
			SkillTBox_10.Text = rnd.Next(0, 101).ToString();		//Combat Ability

			SkillTBox_11.Text = rnd.Next(0, 101).ToString();		//OralSex
			SkillTBox_12.Text = rnd.Next(0, 101).ToString();		//TittySex
			SkillTBox_13.Text = rnd.Next(0, 101).ToString();		//Medicine
			SkillTBox_14.Text = rnd.Next(0, 101).ToString();		//Performance
			SkillTBox_15.Text = rnd.Next(0, 101).ToString();		//Handjob
		}

		//Normalized randomize skills button
		private void button7_Click(object sender, EventArgs e)
		{
			SkillTBox_01.Text = (rnd.Next(0, 21) * 5).ToString();		//Anal Sex
			SkillTBox_02.Text = (rnd.Next(0, 21) * 5).ToString();		//Magic Ability
			SkillTBox_03.Text = (rnd.Next(0, 21) * 5).ToString();		//BDSM Sex
			SkillTBox_04.Text = (rnd.Next(0, 21) * 5).ToString();		//Normal Sex
			SkillTBox_05.Text = (rnd.Next(0, 21) * 5).ToString();		//Bestiality Sex
			SkillTBox_06.Text = (rnd.Next(0, 21) * 5).ToString();		//Group Sex
			SkillTBox_07.Text = (rnd.Next(0, 21) * 5).ToString();		//Lesbian Sex
			SkillTBox_08.Text = (rnd.Next(0, 21) * 5).ToString();		//Service Skills
			SkillTBox_09.Text = (rnd.Next(0, 21) * 5).ToString();		//Stripping Sex
			SkillTBox_10.Text = (rnd.Next(0, 21) * 5).ToString();		//Combat Ability

			SkillTBox_11.Text = (rnd.Next(0, 21) * 5).ToString();		//
			SkillTBox_12.Text = (rnd.Next(0, 21) * 5).ToString();		//
			SkillTBox_13.Text = (rnd.Next(0, 21) * 5).ToString();		//
			SkillTBox_14.Text = (rnd.Next(0, 21) * 5).ToString();		//
			SkillTBox_15.Text = (rnd.Next(0, 21) * 5).ToString();		//
		}

		//Conditional randomize skills button
		private void button8_Click(object sender, EventArgs e)
		{
			SkillTBox_01.Text = (rnd.Next(0, 9) * 5).ToString();		//Anal Sex

			//magic ability, if Strong Magic trait is selected she'll have magic ability 60 or 70, if not it will be random
			if (checkedListBox1.CheckedItems.Count == 0)
			{
				SkillTBox_02.Text = (rnd.Next(0, 9) * 5).ToString();
			}
			else
			{
				int i = 0;
				while (i < checkedListBox1.CheckedItems.Count)
				{
					if (checkedListBox1.CheckedItems[i].ToString() == "Strong Magic")
					{
						if (rnd.Next(2, 9) >= 4) SkillTBox_02.Text = "60";
						else SkillTBox_02.Text = "70";
						break;
					}
					else
					{
						SkillTBox_02.Text = (rnd.Next(0, 9) * 5).ToString();
					}
					i = i + 1;
				}
			}

			SkillTBox_03.Text = (rnd.Next(0, 9) * 5).ToString();		//BDSM Sex

			SkillTBox_04.Text = (rnd.Next(0, 9) * 5).ToString();		//Normal Sex

			SkillTBox_05.Text = (rnd.Next(0, 9) * 5).ToString();		//Bestiality Sex

			SkillTBox_06.Text = (rnd.Next(0, 9) * 5).ToString();		//Group Sex

			SkillTBox_07.Text = (rnd.Next(0, 9) * 5).ToString();		//Lesbian Sex

			SkillTBox_08.Text = (rnd.Next(0, 9) * 5).ToString();		//Service Skills

			SkillTBox_09.Text = (rnd.Next(0, 9) * 5).ToString();		//Stripping Sex
			SkillTBox_11.Text = (rnd.Next(0, 9) * 5).ToString();		//
			SkillTBox_12.Text = (rnd.Next(0, 9) * 5).ToString();		//
			SkillTBox_13.Text = (rnd.Next(0, 9) * 5).ToString();		//
			SkillTBox_14.Text = (rnd.Next(0, 9) * 5).ToString();		//
			SkillTBox_15.Text = (rnd.Next(0, 9) * 5).ToString();		//


			//Combat Ability, again little check if adventurer or assassin trait is set, in that case combat ability will be higher
			if (checkedListBox1.CheckedItems.Count == 0)
			{
				SkillTBox_10.Text = (rnd.Next(0, 9) * 5).ToString();
			}
			else
			{
				int i = 0;
				while (i < checkedListBox1.CheckedItems.Count)
				{
					if (checkedListBox1.CheckedItems[i].ToString() == "Adventurer" || checkedListBox1.CheckedItems[i].ToString() == "Assassin")
					{
						if (rnd.Next(2, 9) >= 4) SkillTBox_10.Text = "60";
						else SkillTBox_10.Text = "70";
						break;
					}
					else
					{
						SkillTBox_10.Text = (rnd.Next(0, 9) * 5).ToString();
					}
					i = i + 1;
				}
			}
		}

		//Resets values in girl tab, I think this doesn't need any expalantion, it resets all values to starting point, and also deselects any girl on girls list, if any is selected
		private void button10_Click(object sender, EventArgs e)
		{
			NameTBox_01.Text = "";
			DescTBox_01.Text = "";
			for (int i = 0; i < checkedListBox1.Items.Count; i++)			   //loop that unchecks every trait in the list
			{
				checkedListBox1.SetItemCheckState(i, CheckState.Unchecked);
			}
			GoldTBox1.Text = "0";
			StatsTBox_01.Text = ""; StatsTBox_02.Text = "100"; StatsTBox_03.Text = ""; StatsTBox_04.Text = ""; StatsTBox_05.Text = ""; StatsTBox_06.Text = ""; StatsTBox_07.Text = ""; StatsTBox_08.Text = ""; StatsTBox_09.Text = "0"; StatsTBox_10.Text = "0"; StatsTBox_11.Text = "0"; StatsTBox_12.Text = "60"; StatsTBox_13.Text = "0"; StatsTBox_14.Text = ""; StatsTBox_15.Text = ""; StatsTBox_16.Text = ""; StatsTBox_17.Text = ""; StatsTBox_18.Text = "0"; StatsTBox_19.Text = "100"; StatsTBox_20.Text = "0"; StatsTBox_21.Text = "0"; StatsTBox_22.Text = "0";
			SkillTBox_01.Text = ""; SkillTBox_02.Text = ""; SkillTBox_03.Text = ""; SkillTBox_04.Text = ""; SkillTBox_05.Text = ""; SkillTBox_06.Text = ""; SkillTBox_07.Text = ""; SkillTBox_08.Text = ""; SkillTBox_09.Text = ""; SkillTBox_10.Text = ""; SkillTBox_11.Text = ""; SkillTBox_12.Text = ""; SkillTBox_13.Text = ""; SkillTBox_14.Text = ""; SkillTBox_15.Text = "";
			comboBox_GirlType.SelectedIndex = 0;
			this.listBox_GirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
			listBox_GirlsList.SelectedItem = null;
			this.listBox_GirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on

			StatusLabel1.Text = "Reset of girl tab values performed...";
		}

		//button that loads existing girls file, also no need to explain in depth, creates OpenFileDialog named Open, sets it's filter to show only *.girls files by default and passes path of selected file to a function that reads girls from file and fills girls dataTable with values
		private void button_LoadGirls_Click(object sender, EventArgs e)
		{
			OpenFileDialog Open = new OpenFileDialog();
			Open.Filter = "Whore Master girls file|*.girlsx; *.girls|All files|*.*";	  //this sets before mentioned filter to show only *.girls files by default
			Open.ShowDialog();
			try
			{
				switch (Path.GetExtension(Open.FileName))
				{
					case ".girls":
						LoadGirls(Open.FileName);									   //calls LoadGirls function and passes file name with path to it
						break;
					case ".girlsx":
						LoadGirlsXML(Open.FileName);
						break;
				}
				SortDataTable(ref GirlsCollection, ref listBox_GirlsList);			  //sorts the list after it's been loaded, this will be kinda redundant "later" when all girls files have passed through updated WMEditor so they'll already sorted, but since there's no way to know if this is the case on particular file...
			}
			catch (Exception err)
			{
				MessageBox.Show(err.Message, "Open trait file error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}

		//method that parses girls files to fill listbox and DataTable
		private void LoadGirls(string path)
		{
			try																		 //concept is same as loading traits, only what it does inside is different
			{
				int x = 0;
				StreamReader Import = new StreamReader(path);
				while (Import.Peek() >= 0)											  //again, do this until EoF, it reads line by line of each girl (they are variable in number of lines, but how variable is specified)
				{
					x++;
					string name = Import.ReadLine();				  //stores line with name in separate string
					string desc = Import.ReadLine();				  //same with description
					string num = Import.ReadLine();				   //next is the number that says how many traits are there (this is why each girl can be different in number of lines from other girls), it could have been imported to int instead of string, but it's used few lines below as string so this way there's one conversion less (there's only one, string to int in for loop, other way it would be one to convert string from file to int so it can be stored as int, and then later int to string to add to dataTable)
					string temp = "";												   //this is where traits are temporarily stored

					for (int i = 0; i < Convert.ToInt32(num); i++)					  //this for loop reads line by line and adds it to temp string, for some reason I didn't use nln thingy as in some previous cases, ergo "\r\n"s
					{
						temp = temp + Convert.ToString(Import.ReadLine()) + "\r\n";
					}

					string zero = Import.ReadLine();				  //this is the line that is currently always "0", maybe it won't be for ever, so better to just parse it as every other line then assume it will always be "0"
					string data1 = Import.ReadLine();				 //line with stats
					string data2 = Import.ReadLine();				 //line with skills
					string gold = Import.ReadLine();				  //gold
					string girlType = Import.ReadLine();			  //type (slave, catacombs...)

					listBox_GirlsList.Items.Add(name);								  //adds girl's name to girls list
					GirlsCollection.Rows.Add(name, desc + "\r\n" + num + "\r\n" + temp + zero + "\r\n" + data1 + "\r\n" + data2 + "\r\n" + gold + "\r\n" + girlType, girlType);	  //adds girls name and data to girls datatable, and GirlType so it can be sorted on that key
				}
				Import.Close();
				StatusLabel1.Text = "Loaded " + x.ToString() + " girls from file...";
			}
			catch /*(Exception err)*/
			{
				//MessageBox.Show(err.Message, "Open error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				StatusLabel1.Text = "Load canceled...";
			}
		}

		//loads girls from XML file
		private void LoadGirlsXML(string path)
		{
			try
			{
				XmlTextReader xmlread = new XmlTextReader(path);					//Creates XmlTextReader instance to open/read XML file

				XmlDocument xmldoc = new XmlDocument();							 //Creates XmlDocument instance that will be used to go through XML file
				xmldoc.Load(xmlread);

				XmlNode baseNode = xmldoc.DocumentElement;						  //This one isolates one node in our XmlDocument

				int x = 0;														  //this is just a counter, it gets incremented for each "girl" read, so at the end we have a number of girls loaded, this is only used to display in status bar how many girls were loaded

				foreach (XmlNode node in baseNode.SelectNodes("/Girls/Girl"))	   //Here we tell it what node we want, basically get collection of all Girl nodes in Girls node, in original XML "Girls" is root node, and each girl has her "Girl" node
				{
					x++;															//here's where we increment our girl counter, getting that number after loading (like geting count of entries in the list) won't work since list may not be empty before this
					string sName = "";				   //girl name, each girl value has it's attribute within it's node, this is how they're read
					string sDesc = "";				   //girl description
					string sData = "";											  //string that will store girl data
					string sSkills = "";											//girl skills
					string sGold = "";				   //starting gold
					string sStatus = "";											//girl status
					ArrayList alTraits = new ArrayList();						   //ArrayList to store parsed traits, later on in the code of this method

					string[] jData = new string[40];
					string[] jSkills = new string[40];

					//now, reason why editor turned out to support conversion to new format is because I made this XML support as a wrapper, data get's read from XML it's stored
					//in memory in old format, this is what next two lines do, they're that long line with bunch of numbers, internally program works the same as before, only new is this XML wrapper

					for (int i = 0; i < node.Attributes.Count; i++)
					{
						if (node.Attributes[i].Name == "Name") sName = node.Attributes["Name"].Value;
						if (node.Attributes[i].Name == "Desc") sDesc = node.Attributes["Desc"].Value;
						if (node.Attributes[i].Name == "Gold") sGold = node.Attributes["Gold"].Value;

						if (node.Attributes[i].Name == "Charisma") jData[0] = node.Attributes["Charisma"].Value;
						if (node.Attributes[i].Name == "Happiness") jData[1] = node.Attributes["Happiness"].Value;
						if (node.Attributes[i].Name == "Libido") jData[2] = node.Attributes["Libido"].Value;
						if (node.Attributes[i].Name == "Constitution") jData[3] = node.Attributes["Constitution"].Value;
						if (node.Attributes[i].Name == "Intelligence") jData[4] = node.Attributes["Intelligence"].Value;
						if (node.Attributes[i].Name == "Confidence") jData[5] = node.Attributes["Confidence"].Value;
						if (node.Attributes[i].Name == "Mana") jData[6] = node.Attributes["Mana"].Value;
						if (node.Attributes[i].Name == "Agility") jData[7] = node.Attributes["Agility"].Value;
						if (node.Attributes[i].Name == "Fame") jData[8] = node.Attributes["Fame"].Value;
						if (node.Attributes[i].Name == "Level") jData[9] = node.Attributes["Level"].Value;
						if (node.Attributes[i].Name == "AskPrice") jData[10] = node.Attributes["AskPrice"].Value;
						if (node.Attributes[i].Name == "House") jData[11] = node.Attributes["House"].Value;
						if (node.Attributes[i].Name == "Exp") jData[12] = node.Attributes["Exp"].Value;
						if (node.Attributes[i].Name == "Age") jData[13] = node.Attributes["Age"].Value;
						if (node.Attributes[i].Name == "Obedience") jData[14] = node.Attributes["Obedience"].Value;
						if (node.Attributes[i].Name == "Spirit") jData[15] = node.Attributes["Spirit"].Value;
						if (node.Attributes[i].Name == "Beauty") jData[16] = node.Attributes["Beauty"].Value;
						if (node.Attributes[i].Name == "Tiredness") jData[17] = node.Attributes["Tiredness"].Value;
						if (node.Attributes[i].Name == "Health") jData[18] = node.Attributes["Health"].Value;
						if (node.Attributes[i].Name == "PCFear") jData[19] = node.Attributes["PCFear"].Value;
						if (node.Attributes[i].Name == "PCLove") jData[20] = node.Attributes["PCLove"].Value;
						if (node.Attributes[i].Name == "PCHate") jData[21] = node.Attributes["PCHate"].Value;
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
						if (node.Attributes[i].Name == "Status")
						{
							switch (node.Attributes["Status"].Value)		//check to read girl status flag (and translate that to old format)
							{
								case "Slave":
									sStatus = "1";
									break;
								case "Catacombs":
									sStatus = "2";
									break;
								case "Arena":
									sStatus = "3";
									break;
                                case "Normal":
                                default:
                                    sStatus = "0";
                                    break;

                            }

						}

					}
                    // `J` set defaults if they are not in the girlsx file
                    if (sDesc == null) sDesc = "-";
                    if (sGold == null) sGold = "0";
                    if (sStatus == null) sStatus = "0";
                    if (jData[1] == null) jData[1] = "100";
                    if (jData[18] == null) jData[18] = "100";
                    if (jData[11] == null) jData[11] = "60";
                    for (int i = 0; i < jData.Length;i++)
                    {
                        if (jData[i] != null)
                            sData += jData[i] + " ";
                        else sData += "0 ";
                    }
                    for (int i = 0; i < jSkills.Length; i++)
                    {
                        if (jSkills[i] != null)
                            sSkills += jSkills[i] + " ";
                        else sSkills += "0 ";
                    }

                    /*  next are attributes, they're child node of parent "Girl" node, 
                     * simply check if there are any, 
                     * if yes go through all of them to store each one in alTraits ArrayList
                    // */ //
                    if (node.HasChildNodes == true)				 
                    {
                        for (int y = 0; y < node.ChildNodes.Count; y++)
                        {
                            alTraits.Add(node.ChildNodes[y].Attributes["Name"].Value);
                        }
                    }

                    /*  old format requires that there's a number of traits, this is it. 
                     * This string will store all traits, so first we put number of traits in it
                    // */ //
                    string sTraits = alTraits.Count.ToString();	 

					for (int y = 0; y < alTraits.Count; y++)		//and then go through all of them in alTraits and add them to sTraits string. When it's done it'll contain traits in the old format
					{											   //now that I think about it, this could have been done directly in the part that puts them in alTraits and skip that alTraits completely
						sTraits = sTraits + "\r\n" + alTraits[y];   //but it works as it is and I'm to lazy to change that now :P
					}

					//when we get to this part one girl has been read from XML and data collected, now we only need to store that data in DataTable in old format, this is what next line does, the one behind it adds her name to girls list
					GirlsCollection.Rows.Add(sName, sDesc + "\r\n" + sTraits + "\r\n" + "0" + "\r\n" + sData + "\r\n" + sSkills + "\r\n" + sGold + "\r\n" + sStatus, sStatus);
					listBox_GirlsList.Items.Add(sName);
				}
				xmlread.Close();
				StatusLabel1.Text = "Loaded " + x.ToString() + " girls from XML file...";	   //and here's where that "x" with number of girls is used
			}
			catch (Exception err)
			{
				MessageBox.Show(err.Message, "XML load error error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				StatusLabel1.Text = "XML load error...";
			}
		}

		//Delete selected girl from the list
		private void button_DeleteGirl_Click(object sender, EventArgs e)
		{
			if (comboBox_SortByType.SelectedItem.ToString() == "All")
			{
				try
				{
					this.listBox_GirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
					StatusLabel1.Text = "\"" + listBox_GirlsList.SelectedItem.ToString() + "\"" + " deleted...";
					GirlsCollection.Rows[listBox_GirlsList.SelectedIndex].Delete();				 //again, since dataTable and list index are the same it's easy to find which row to delete
					listBox_GirlsList.Items.RemoveAt(listBox_GirlsList.SelectedIndex);
					this.listBox_GirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
				}
				catch (Exception err)
				{
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
					GirlsCollection.Rows[(int)GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex][3]].Delete();				 //first we need to delete entry from the original because if we delete temp DataTable entry we loose the way to identify which row in original needs to be deleted
					GirlsCollectionTemp.Rows[listBox_GirlsList.SelectedIndex].Delete();			 //since temp dataTable and list index in filtered case are the same it's easy to find which row to delete
					listBox_GirlsList.Items.RemoveAt(listBox_GirlsList.SelectedIndex);
					this.listBox_GirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_GirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
				}
				catch (Exception err)
				{
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
			StatusLabel1.Text = "Girl list cleared...";
		}

		//button that compiles girls data from dataTable and saves it to a file
		private void button_SaveGirls_Click(object sender, EventArgs e)
		{
			Filesave.FileName = "";
			Filesave.Filter = "Whore Master XML girls file|*.girlsx|All files|*.*";
			Filesave.ShowDialog();

			try
			{
				if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
				if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");
				SaveGirlsXML(Filesave.FileName);
				StatusLabel1.Text = "Successfully compiled " + listBox_GirlsList.Items.Count.ToString() + " girls...";
			}
			catch 
			{
			} 

/*			comboBox_SortByType.SelectedIndex = 0;					  //change filter to "all", this invokes SelecectedIndexChanged event and performs what's been specified for "all" case, in effect it resorts out DataTable before the save
			Filesave.FileName = "";									 //if some file was saved previously it's name will be stored, so this clears that
			Filesave.Filter = "Whore Master XML girls file|*.girlsx|Whore Master girls file|*.girls|All files|*.*";

			Filesave.ShowDialog();								  //save procedure, open dialog window, get file name user entered, pass it to StreamWriter who will use it to create this file, close file
			if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
			if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");

			try
			{
				switch (Path.GetExtension(Filesave.FileName))
				{
					case ".girls":
						SaveGirls(Filesave.FileName);
						break;
					case ".girlsx":
						SaveGirlsXML(Filesave.FileName);
						break;
				}
			}
			catch
			{
			}
			StatusLabel1.Text = "Successfully compiled " + listBox_GirlsList.Items.Count.ToString() + " girls...";
 */
		}

		//save girls to old format
		private void SaveGirls(string path)
		{
			string output = "";										 //this is the string to which output from following for loop will be saved, more precisely what will be save in final file

			for (int i = 0; i < listBox_GirlsList.Items.Count; i++)	 //again, no philosophy in this one, get the number of girls in the list, and for every one of them do the following, which is take name from dataTable and add it to output, "press" enter, add data from dataTable, this time there's no need for "enter", girl data string has one at the end, it turned out to be best this way, no need to worry if new girl added to the output will be in new line, this way it's automatic, although this is not necessarily the best way
			{
				if (listBox_GirlsList.Items.Count == i + 1) output = output + GirlsCollection.Rows[i][0] + "\r\n" + GirlsCollection.Rows[i][1];
				else output = output + GirlsCollection.Rows[i][0] + "\r\n" + GirlsCollection.Rows[i][1] + "\r\n";
			}
			output = output.TrimEnd('\r', '\n');						//when whole output string is done there will be an extra "enter" ("\r\n") at the end from the last girl dataTable data, this will mess up the game if there's an extra empty line at the end, but since I KNOW that it's there it's easily removable, this line does that. This could have been done differently, like remove that last "\r\n" from girl data, and then add it through previous loop with check that in case it's last girl don't add "\r\n" at the end, but this works, and it's one if less

			StreamWriter Export = new StreamWriter(Convert.ToString(Filesave.FileName));
			Export.Write(output);
			Export.Close();

		}

		//save girls to XML format
		private void SaveGirlsXML(string path)
		{																   //and this is other part of the wrapper, this one reads old format from memory and stores it in XML
			XmlDocument xmldoc = new XmlDocument();						 //first we'll create XmlDocument

			XmlElement girls = xmldoc.CreateElement("Girls");			   //and create nodes that are used in this document
			XmlElement girl = xmldoc.CreateElement("Girl");
			XmlElement trait = xmldoc.CreateElement("Trait");

			xmldoc.AppendChild(girls);									  //"Girls" is the root node so we append it to our xmldoc first

			for (int x = 0; x < GirlsCollection.Rows.Count; x++)			//and now we need to go through each girl, i.e. row in DataTable
			{
				girl = xmldoc.CreateElement("Girl");						//I think that creation of child nodes could be placed here instead of there, but this way code looks more readable, anyway, here's where we create our "Girl" node for each girl

				StringReader sData = new StringReader(GirlsCollection.Rows[x][1].ToString());	   //girl data is stored in string that's stored in second column of each row in DataTable, to be able to read it easier we'll access that through StringReader object, it has similar properties as TextReader

				string sName = GirlsCollection.Rows[x][0].ToString();	   //girl name is stored in first column of the row so we can get to it directly
				string sDesc = sData.ReadLine();							//girl description is first line of description

				int num = Convert.ToInt32(sData.ReadLine());				//after description we get to traits, first line after description is number of traits

				for (int y = 0; y < num; y++)							   //we'll use it to know how many lines need to be read (i.e. how many lines are traits)
				{
					trait = xmldoc.CreateElement("Trait");				  //each trait is stored in separate child node under "Girl" node, so here's where that's done

					trait.SetAttribute("Name", sData.ReadLine());		   //Trait name is attribute in "Trait" node, this is where that attribute get's set
					girl.AppendChild(trait);								//and after that's done append that child node to "Girl" node, rinse and repeat for every trait
				}

				string sZero = sData.ReadLine();							//This line is always 0, it's not used for now, but if need be it's stored anyway, it's just not currently saved to XML (there are no attribute for it)

				string[] sStats = sData.ReadLine().Split(' ');			  //line with stats, stored in string array, it's basically space delimited data so it get's splitted right away
				string[] sSkills = sData.ReadLine().Split(' ');			 //and again for skills
				string sGold = sData.ReadLine();							//gold
				string sStatus = sData.ReadLine();						  //status, slave, normal, catacombs, arena

				switch (sStatus)											//and here's where it gets "translated" to old XML, on old format it's represented with number, in new with string, so this simple replaces that number with string
				{
					case "1":
						sStatus = "Slave";
						break;
					case "2":
						sStatus = "Catacombs";
						break;
                    case "3":
                        sStatus = "Arena";
                        break;
                    case "0":
                    default:
                        sStatus = "Normal";
                        break;
                }
				
				//now that we have all that data isolated it should be assigned to their respective attributes in "Girl" node, this is what's done here, bunch of attributes getting assingned their values

				girl.SetAttribute("Name", sName);
				girl.SetAttribute("Desc", sDesc);
				girl.SetAttribute("Charisma", sStats[0]);
				girl.SetAttribute("Happiness", sStats[1]);
				girl.SetAttribute("Libido", sStats[2]);
				girl.SetAttribute("Constitution", sStats[3]);
				girl.SetAttribute("Intelligence", sStats[4]);
				girl.SetAttribute("Confidence", sStats[5]);
				girl.SetAttribute("Mana", sStats[6]);
				girl.SetAttribute("Agility", sStats[7]);
				girl.SetAttribute("Fame", sStats[8]);
				girl.SetAttribute("Level", sStats[9]);
				girl.SetAttribute("AskPrice", sStats[10]);
				girl.SetAttribute("House", sStats[11]);
				girl.SetAttribute("Exp", sStats[12]);
				girl.SetAttribute("Age", sStats[13]);
				girl.SetAttribute("Obedience", sStats[14]);
				girl.SetAttribute("Spirit", sStats[15]);
				girl.SetAttribute("Beauty", sStats[16]);
				girl.SetAttribute("Tiredness", sStats[17]);
				girl.SetAttribute("Health", sStats[18]);
				girl.SetAttribute("PCFear", sStats[19]);
				girl.SetAttribute("PCLove", sStats[20]);
				girl.SetAttribute("PCHate", sStats[21]);

				girl.SetAttribute("Anal", sSkills[0]);
				girl.SetAttribute("Magic", sSkills[1]);
				girl.SetAttribute("BDSM", sSkills[2]);
				girl.SetAttribute("NormalSex", sSkills[3]);
				girl.SetAttribute("Beastiality", sSkills[4]);
				girl.SetAttribute("Group", sSkills[5]);
				girl.SetAttribute("Lesbian", sSkills[6]);
				girl.SetAttribute("Service", sSkills[7]);
				girl.SetAttribute("Strip", sSkills[8]);
				girl.SetAttribute("Combat", sSkills[9]);
				girl.SetAttribute("OralSex", sSkills[10]);
				girl.SetAttribute("TittySex", sSkills[11]);
				girl.SetAttribute("Medicine", sSkills[12]);
				girl.SetAttribute("Performance", sSkills[13]);
				girl.SetAttribute("Handjob", sSkills[14]);

				girl.SetAttribute("Gold", sGold);
				girl.SetAttribute("Status", sStatus);

				girls.AppendChild(girl);								//finaly done (with one girl), all her data has been read and assigned to nodes or attributes, now we only need to "stick" this finished "Girl" child node to root "Girls" node and this is what this does. and then goes at begining of the loop for the next one
			}
																		//after it's all done we have our XML, although, only in memory, not stored somewhere safe
			XmlWriterSettings settings = new XmlWriterSettings();	   //I've tried few ways of saving this, and this had the nicest output (they were all correct XML wise mind you, but output of this has the "nicest" structure as far as human readability goes
			settings.Indent = true;									 //indent every node, otherwise it would be harder to find where each node begins, again, not for computer, for some person looking at outputed XML
			settings.NewLineOnAttributes = true;						//without this each node would be one long line, this puts each attribute in new line
			settings.IndentChars = "\t";								//just a character that'll be used for indenting, \t means tab, so indent is one tab,
			XmlWriter xmlwrite = XmlWriter.Create(path, settings);	  //now that settings are complete we can write this file, using path passed from button function, and settings we just made

			xmldoc.Save(xmlwrite);									  //now we tell our XmlDocument to save itself to our XmlWriter, this is what finally gives us our file
			xmlwrite.Close();										   //now to be all nice and proper we close our file, after all it's finished
		}

		//this happens when you select something from girl list listBox, that invokes SelectedIndexChanged event and values from entry with this index get filled to item tab boxes
		private void listBox_GirlsList_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (comboBox_SortByType.SelectedItem.ToString() == "All")
			{
				GirlChanged(ref GirlsCollection);
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Normal girls")
			{
				GirlChanged(ref GirlsCollectionTemp);
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Slave girls")
			{
				GirlChanged(ref GirlsCollectionTemp);
			}
            else if (comboBox_SortByType.SelectedItem.ToString() == "Catacombs girls")
            {
                GirlChanged(ref GirlsCollectionTemp);
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Arena girls")
            {
                GirlChanged(ref GirlsCollectionTemp);
            }
        }

		//to allow the previous event to function with different DataTables I've moved it to separate function that gets called with DataTable as parameter, this was needed because of sorting feature
		private void GirlChanged(ref DataTable dt)
		{
			NameTBox_01.Text = dt.Rows[listBox_GirlsList.SelectedIndex][0].ToString();	 //what goes on is basically compiling in reverse, name is already isolated so it just needs to be put in it's place
			StringReader data = new StringReader(dt.Rows[listBox_GirlsList.SelectedIndex][1].ToString());	  //data is basically chunk of text, as it's in file, so we construct StringReader around that data, and proceed to read it and parse it line by line
			DescTBox_01.Text = data.ReadLine();								 //first line is description, just pass it through to it's textbox

			int traitNum = Convert.ToInt32(data.ReadLine());					//next is number of traits, this isn't entered anywhere, upon compiling it's caluculated anew, it's used for following for loop to read all traits from data

			for (int i = 0; i < checkedListBox1.Items.Count; i++)			   //well, not this "for" loop... this "for" loop set's every trait to unchecked state, or else they would get mixed while browsing the girls, or you'd have to click reset button before selecting new girl
			{
				checkedListBox1.SetItemChecked(i, false);
			}

			for (int i = 0; i < traitNum; i++)								  //two nested for loops, first one reads trait, second one looks in trait list for that trait, when it finds it it checks it, and then first one loads next one, so this one checks it and so on...
			{
				string trait = data.ReadLine();
				for (int j = 0; j < checkedListBox1.Items.Count; j++)
				{
					if (checkedListBox1.Items[j].ToString() == trait)
					{
						checkedListBox1.SetItemChecked(j, true);
						break;
					}
				}
			}

			data.ReadLine();														//this line is always 0, it isn't displayed anywhere so we'll just flush it

			char[] separator = { ' ' };											 //data in item data is space delimited, so to begin extraction from it we create an array of separator characters
			string[] values = data.ReadLine().Split(separator);					 //use Split function to put every value that's "between" spaces to array

			//and now fill stats boxes with values from previous array
			if (values.Length > 00) StatsTBox_01.Text = values[0];  else StatsTBox_01.Text = "0";
			if (values.Length > 01) StatsTBox_02.Text = values[1];  else StatsTBox_02.Text = "0";
			if (values.Length > 02) StatsTBox_03.Text = values[2];  else StatsTBox_03.Text = "0";
			if (values.Length > 03) StatsTBox_04.Text = values[3];  else StatsTBox_04.Text = "0";
			if (values.Length > 04) StatsTBox_05.Text = values[4];  else StatsTBox_05.Text = "0";
			if (values.Length > 05) StatsTBox_06.Text = values[5];  else StatsTBox_06.Text = "0";
			if (values.Length > 06) StatsTBox_07.Text = values[6];  else StatsTBox_07.Text = "0";
			if (values.Length > 07) StatsTBox_08.Text = values[7];  else StatsTBox_08.Text = "0";
			if (values.Length > 08) StatsTBox_09.Text = values[8];  else StatsTBox_09.Text = "0";
			if (values.Length > 09) StatsTBox_10.Text = values[9];  else StatsTBox_10.Text = "0";
			if (values.Length > 10) StatsTBox_11.Text = values[10]; else StatsTBox_11.Text = "0";
			if (values.Length > 11) StatsTBox_12.Text = values[11]; else StatsTBox_12.Text = "0";
			if (values.Length > 12) StatsTBox_13.Text = values[12]; else StatsTBox_13.Text = "0";
			if (values.Length > 13) StatsTBox_14.Text = values[13]; else StatsTBox_14.Text = "0";
			if (values.Length > 14) StatsTBox_15.Text = values[14]; else StatsTBox_15.Text = "0";
			if (values.Length > 15) StatsTBox_16.Text = values[15]; else StatsTBox_16.Text = "0";
			if (values.Length > 16) StatsTBox_17.Text = values[16]; else StatsTBox_17.Text = "0";
			if (values.Length > 17) StatsTBox_18.Text = values[17]; else StatsTBox_18.Text = "0";
			if (values.Length > 18) StatsTBox_19.Text = values[18]; else StatsTBox_19.Text = "0";
			if (values.Length > 19) StatsTBox_20.Text = values[19]; else StatsTBox_20.Text = "0";
			if (values.Length > 20) StatsTBox_21.Text = values[20]; else StatsTBox_21.Text = "0";
			if (values.Length > 21) StatsTBox_22.Text = values[21]; else StatsTBox_22.Text = "0";

			values = data.ReadLine().Split(separator);							  
			//again for skills
			if (values.Length > 00) SkillTBox_01.Text = values[0];  else SkillTBox_01.Text = "0";
			if (values.Length > 01) SkillTBox_02.Text = values[1];  else SkillTBox_02.Text = "0";
			if (values.Length > 02) SkillTBox_03.Text = values[2];  else SkillTBox_03.Text = "0";
			if (values.Length > 03) SkillTBox_04.Text = values[3];  else SkillTBox_04.Text = "0";
			if (values.Length > 04) SkillTBox_05.Text = values[4];  else SkillTBox_05.Text = "0";
			if (values.Length > 05) SkillTBox_06.Text = values[5];  else SkillTBox_06.Text = "0";
			if (values.Length > 06) SkillTBox_07.Text = values[6];  else SkillTBox_07.Text = "0";
			if (values.Length > 07) SkillTBox_08.Text = values[7];  else SkillTBox_08.Text = "0";
			if (values.Length > 08) SkillTBox_09.Text = values[8];  else SkillTBox_09.Text = "0";
			if (values.Length > 09) SkillTBox_10.Text = values[9];  else SkillTBox_10.Text = "0";
			if (values.Length > 10) SkillTBox_11.Text = values[10]; else SkillTBox_11.Text = "0";
			if (values.Length > 12) SkillTBox_12.Text = values[11]; else SkillTBox_12.Text = "0";
			if (values.Length > 13) SkillTBox_13.Text = values[12]; else SkillTBox_13.Text = "0";
			if (values.Length > 14) SkillTBox_14.Text = values[13]; else SkillTBox_14.Text = "0";
			if (values.Length > 15) SkillTBox_15.Text = values[14]; else SkillTBox_15.Text = "0";

			GoldTBox1.Text = data.ReadLine();									   //gold

			string gtype = data.ReadLine();										 //and girl type

			if (Convert.ToInt32(gtype) < 0 || Convert.ToInt32(gtype) > 3)		   //Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Normal Girl" if value is out of bounds
			{
				comboBox_GirlType.SelectedIndex = 0;
				StatusLabel1.Text = "\"" + NameTBox_01.Text + "\"" + " parsed successfully, girl type value was out of bounds, defaulted to \"Normal girl\"...";
			}
			else
			{
				comboBox_GirlType.SelectedIndex = Convert.ToInt32(gtype);		   //if it's "within bounds" just convert string to number and make that item in droplist selected
				StatusLabel1.Text = "\"" + NameTBox_01.Text + "\"" + " parsed successfully...";
			}
		}

		//displays tooltip on trait entries on girls tab, something I scrounged from the internet
		private void checkedListBox1_MouseMove(object sender, MouseEventArgs e)
		{
			ListBox listBox = (ListBox)sender;								  //creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
			int index = listBox.IndexFromPoint(e.Location);					 //e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
			if (index > -1 && index < listBox.Items.Count)					  //if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
			{
				string tip = listBox.Items[index].ToString();				   //this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
				if (tip != lastTip)											 //every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
				{															   //also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
					toolTip1.SetToolTip(listBox, aTraits[index].ToString());	//assigns tooltip at index "index" from atraits arraylist
					lastTip = tip;											  //updates lastTip string
				}
			}
		}

		//displays tooltips with girl descriptions on girls list, same as above with traits
		private void listBox_GirlsList_MouseMove(object sender, MouseEventArgs e)
		{
			ListBox listBox = (ListBox)sender;								  //creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
			int index = listBox.IndexFromPoint(e.Location);					 //e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
			if (index > -1 && index < listBox.Items.Count)					  //if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
			{
				string tip = listBox.Items[index].ToString();				   //this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
				if (tip != lastTip)											 //every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
				{															   //also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
					toolTip1.SetToolTip(listBox, new StringReader(GirlsCollection.Rows[index][1].ToString()).ReadLine());	//this is the only difference from previous tooltip function. Description is stored in dataTable at [index][1], along with other data, so to get it out we first need to identify coordinates (that's what [index][1] is for), then construct StringReader from that long string, and lastly read first line from that stream, this is description. Luckily it can be fitted in one line :P
					lastTip = tip;											  //updates lastTip string
				}
			}
		}

		//toggles display of tooltips for traits list on girl tab
		private void checkBox_ToggleTraitTooltips_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox_ToggleTraitTooltips.Checked == false)
			{
				checkedListBox1.MouseMove -= new System.Windows.Forms.MouseEventHandler(checkedListBox1_MouseMove);
				toolTip1.SetToolTip(checkedListBox1, "");
			}
			else checkedListBox1.MouseMove += new System.Windows.Forms.MouseEventHandler(checkedListBox1_MouseMove);

		}

		//dropbox to select filtering by type, simply sends different parameters for different cases of drop boxs, except in first case, it just performs regular sort to repopulate the list
		private void comboBox_SortByType_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (comboBox_SortByType.SelectedItem.ToString() == "All")
			{
				SortDataTable(ref GirlsCollection, ref listBox_GirlsList);
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Normal girls")
			{
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "0");
			}
			else if (comboBox_SortByType.SelectedItem.ToString() == "Slave girls")
			{
				FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "1");
			}
            else if (comboBox_SortByType.SelectedItem.ToString() == "Catacombs girls")
            {
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "2");
            }
            else if (comboBox_SortByType.SelectedItem.ToString() == "Arena girls")
            {
                FilterGirls(GirlsCollection, ref GirlsCollectionTemp, ref listBox_GirlsList, "3");
            }
        }

		//this is filtering function that previous event uses, I'll probably upgrade it to filter everything needed
		private void FilterGirls(DataTable dt, ref DataTable tempDT, ref ListBox lb, string sFilter)
		{
			tempDT.Reset();											 //resets temp DataTable
			tempDT = dt.Clone();										//copies the structure from original DataTable

			for (int x = 0; x < dt.Rows.Count; x++) tempDT.ImportRow(dt.Rows[x]);	   //this copies content from original DataTable

			tempDT.Columns.Add("ID", typeof(int));					  //adds a column to temp DataTable, this is where link with original DataTable will be stored

			for (int x = 0; x < tempDT.Rows.Count; x++)				 //at this moment temp DataTable is still exact copy of original, so we fill new ID column with number that represents current row number
			{														   //after we filter it they will remain with current values that will tell us what line in original DataTable needs to be updated
				tempDT.Rows[x][3] = x;
			}

			DataView v = tempDT.DefaultView;							//rest is almost the same as with normal sort, create DataView from our DataTable, this time temp DataTable that will get gutted :P, so our original doesn't loose any data
			v.RowFilter = "TypeFilter = '" + sFilter + "'";			 //this is the only real change from normal sort, we simply say by which column it needs to filter and by what value, At this moment I've adapted other functions to put type in TypeFilter, actual value it filters comes from where this function is called
			v.Sort = "Name ASC";										//sort this DataView in ascending order using "Name" column as key
			tempDT = v.ToTable();									   //apply this sorted view to our original DataTable

			lb.Items.Clear();										   //empty listbBox from entries it has

			for (int x = 0; x < tempDT.Rows.Count; x++)				 //go through all records in DataTable and add names to listBox so our index sync works again
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
			catch
			{
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
			string[] sRGirl = { "", "" };	   //as with girls, this is string array that will store name and data
			string nln = "\r\n";
			string sp = " ";
			string sRTraits = "";			   //traits for random girls
			string sRStatsMin = "";			 //line with minimum value for stats
			string sRStatsMax = "";			 //line with maximum value for stats
			string sRSkillsMin = "";			//minimum skills
			string sRSkillsMax = "";			//maximum skills
			string sLine9 = "";				 //this was previously unsued line9, now it shows if girl is human or not
			string sLine10 = "";				//line 10, manual says this should be 0, but it seems it's catacombs flag, although currently it doesn't do anything
			string sRGDesc;					 //random girl description

			if (checkBox_RGHuman.Checked == false) sLine9 = "1";
			else sLine9 = "0";

			if (checkBox_RGCatacombs.Checked == true) sLine10 = "1";
			else sLine10 = "0";

			int i = 0;
			while (i < dataGridView1.Rows.Count)
			{
				sRTraits = sRTraits + nln + dataGridView1.Rows[i].Cells[0].Value.ToString() + nln + dataGridView1.Rows[i].Cells[1].Value.ToString();
				i = i + 1;
			}

			sRStatsMin = StatRGMinTBox1.Text + sp + StatRGMinTBox2.Text + sp + StatRGMinTBox3.Text + sp + StatRGMinTBox4.Text + sp + StatRGMinTBox5.Text + sp + StatRGMinTBox6.Text + sp + StatRGMinTBox7.Text + sp + StatRGMinTBox8.Text + sp + StatRGMinTBox9.Text + sp + StatRGMinTBox10.Text + sp + StatRGMinTBox11.Text + sp + StatRGMinTBox12.Text + sp + StatRGMinTBox13.Text + sp + StatRGMinTBox14.Text + sp + StatRGMinTBox15.Text + sp + StatRGMinTBox16.Text + sp + StatRGMinTBox17.Text + sp + StatRGMinTBox18.Text + sp + StatRGMinTBox19.Text + sp + StatRGMinTBox20.Text + sp + StatRGMinTBox21.Text + sp + StatRGMinTBox22.Text;
			sRStatsMax = StatRGMaxTBox1.Text + sp + StatRGMaxTBox2.Text + sp + StatRGMaxTBox3.Text + sp + StatRGMaxTBox4.Text + sp + StatRGMaxTBox5.Text + sp + StatRGMaxTBox6.Text + sp + StatRGMaxTBox7.Text + sp + StatRGMaxTBox8.Text + sp + StatRGMaxTBox9.Text + sp + StatRGMaxTBox10.Text + sp + StatRGMaxTBox11.Text + sp + StatRGMaxTBox12.Text + sp + StatRGMaxTBox13.Text + sp + StatRGMaxTBox14.Text + sp + StatRGMaxTBox15.Text + sp + StatRGMaxTBox16.Text + sp + StatRGMaxTBox17.Text + sp + StatRGMaxTBox18.Text + sp + StatRGMaxTBox19.Text + sp + StatRGMaxTBox20.Text + sp + StatRGMaxTBox21.Text + sp + StatRGMaxTBox22.Text;

			sRSkillsMin = SkillRGMinTBox1.Text + sp + SkillRGMinTBox2.Text + sp + SkillRGMinTBox3.Text + sp + SkillRGMinTBox4.Text + sp + SkillRGMinTBox5.Text + sp + SkillRGMinTBox6.Text + sp + SkillRGMinTBox7.Text + sp + SkillRGMinTBox8.Text + sp + SkillRGMinTBox9.Text + sp + SkillRGMinTBox10.Text;
			sRSkillsMax = SkillRGMaxTBox1.Text + sp + SkillRGMaxTBox2.Text + sp + SkillRGMaxTBox3.Text + sp + SkillRGMaxTBox4.Text + sp + SkillRGMaxTBox5.Text + sp + SkillRGMaxTBox6.Text + sp + SkillRGMaxTBox7.Text + sp + SkillRGMaxTBox8.Text + sp + SkillRGMaxTBox9.Text + sp + SkillRGMaxTBox10.Text;

			if (descRTBox1.Text.Length == 0) sRGDesc = "-";
			else sRGDesc = descRTBox1.Text;

			sRGirl[0] = nameRTBox1.Text;
			sRGirl[1] = sRGDesc + nln + sRStatsMin + nln + sRStatsMax + nln + sRSkillsMin + nln + sRSkillsMax + nln + GoldRMinTBox1.Text + nln + GoldRMaxTBox1.Text + nln + sLine9 + nln + sLine10 + nln + dataGridView1.Rows.Count + sRTraits;

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
			ValidateRG(GoldRMinTBox1, GoldRMaxTBox1);
		}

		//random "function" for random girls, in regular girl randomize it's kinda tolerable to run these directly in button press event, but since in this case there are two textboxes to fill (and one value is dependant on another) it's easier to manage if they're done through custom function that I just call in button press event, and if I need to adjust it I just have one place to adjust
		private void RandomRG(TextBox min, TextBox max, int x, int y, Random rnd)
		{
			min.Text = rnd.Next(x, y).ToString();
			max.Text = rnd.Next(Convert.ToInt32(min.Text), y).ToString();
		}

		//random "function" for normalized values for random girls
		private void RandomRGNorm(TextBox min, TextBox max, int x, int y, Random rnd, int multi)
		{
			min.Text = (rnd.Next(x, y) * multi).ToString();
			max.Text = (rnd.Next(Convert.ToInt32(min.Text) / multi, y) * multi).ToString();
		}

		//Randomize stats on random girl tab
		private void buttonRandomizeRG1_Click(object sender, EventArgs e)
		{
			RandomRG(StatRGMinTBox1, StatRGMaxTBox1, 1, 101, rnd);		  //Charisma
			RandomRG(StatRGMinTBox3, StatRGMaxTBox3, 1, 101, rnd);		  //Libido
			RandomRG(StatRGMinTBox4, StatRGMaxTBox4, 1, 101, rnd);		  //Constitution
			RandomRG(StatRGMinTBox5, StatRGMaxTBox5, 1, 101, rnd);		  //Intelligence
			RandomRG(StatRGMinTBox6, StatRGMaxTBox6, 1, 101, rnd);		  //Confidence
			RandomRG(StatRGMinTBox7, StatRGMaxTBox7, 1, 101, rnd);		  //Mana
			RandomRG(StatRGMinTBox8, StatRGMaxTBox8, 1, 101, rnd);		  //Agility
			RandomRG(StatRGMinTBox14, StatRGMaxTBox14, 18, 101, rnd);	   //Age
			RandomRG(StatRGMinTBox15, StatRGMaxTBox15, 1, 101, rnd);		//Obedience
			RandomRG(StatRGMinTBox16, StatRGMaxTBox16, 1, 101, rnd);		//Spirit
			RandomRG(StatRGMinTBox17, StatRGMaxTBox17, 1, 101, rnd);		//Beauty  
		}

		//Randomize skills on random girl tab
		private void buttonRandomizeRG2_Click(object sender, EventArgs e)
		{
			RandomRG(SkillRGMinTBox1, SkillRGMaxTBox1, 0, 101, rnd);		//Anal Sex
			RandomRG(SkillRGMinTBox2, SkillRGMaxTBox2, 0, 101, rnd);		//Magic Ability
			RandomRG(SkillRGMinTBox3, SkillRGMaxTBox3, 0, 101, rnd);		//BDSM Sex
			RandomRG(SkillRGMinTBox4, SkillRGMaxTBox4, 0, 101, rnd);		//Normal Sex
			RandomRG(SkillRGMinTBox5, SkillRGMaxTBox5, 0, 101, rnd);		//Bestiality Sex
			RandomRG(SkillRGMinTBox6, SkillRGMaxTBox6, 0, 101, rnd);		//Group Sex
			RandomRG(SkillRGMinTBox7, SkillRGMaxTBox7, 0, 101, rnd);		//Lesbian Sex
			RandomRG(SkillRGMinTBox8, SkillRGMaxTBox8, 0, 101, rnd);		//Service Skills
			RandomRG(SkillRGMinTBox9, SkillRGMaxTBox9, 0, 101, rnd);		//Stripping Sex
			RandomRG(SkillRGMinTBox10, SkillRGMaxTBox10, 0, 101, rnd);	  //Combat Ability
		}

		//resets values in input boxes on random girls tab
		private void button17_Click(object sender, EventArgs e)
		{
			nameRTBox1.Text = "";
			descRTBox1.Text = "";
			rgTable.Clear();
			GoldRMinTBox1.Text = "0"; GoldRMaxTBox1.Text = "10";
			StatRGMinTBox1.Text = ""; StatRGMinTBox2.Text = "100"; StatRGMinTBox3.Text = ""; StatRGMinTBox4.Text = ""; StatRGMinTBox5.Text = ""; StatRGMinTBox6.Text = ""; StatRGMinTBox7.Text = ""; StatRGMinTBox8.Text = ""; StatRGMinTBox9.Text = "0"; StatRGMinTBox10.Text = "0"; StatRGMinTBox11.Text = "0"; StatRGMinTBox12.Text = "100"; StatRGMinTBox13.Text = "0"; StatRGMinTBox14.Text = ""; StatRGMinTBox15.Text = ""; StatRGMinTBox16.Text = ""; StatRGMinTBox17.Text = ""; StatRGMinTBox18.Text = "0"; StatRGMinTBox19.Text = "100"; StatRGMinTBox20.Text = "0"; StatRGMinTBox21.Text = "0"; StatRGMinTBox22.Text = "0";
			SkillRGMinTBox1.Text = ""; SkillRGMinTBox2.Text = ""; SkillRGMinTBox3.Text = ""; SkillRGMinTBox4.Text = ""; SkillRGMinTBox5.Text = ""; SkillRGMinTBox6.Text = ""; SkillRGMinTBox7.Text = ""; SkillRGMinTBox8.Text = ""; SkillRGMinTBox9.Text = ""; SkillRGMinTBox10.Text = "";
			StatRGMaxTBox1.Text = ""; StatRGMaxTBox2.Text = "100"; StatRGMaxTBox3.Text = ""; StatRGMaxTBox4.Text = ""; StatRGMaxTBox5.Text = ""; StatRGMaxTBox6.Text = ""; StatRGMaxTBox7.Text = ""; StatRGMaxTBox8.Text = ""; StatRGMaxTBox9.Text = "0"; StatRGMaxTBox10.Text = "0"; StatRGMaxTBox11.Text = "0"; StatRGMaxTBox12.Text = "100"; StatRGMaxTBox13.Text = "0"; StatRGMaxTBox14.Text = ""; StatRGMaxTBox15.Text = ""; StatRGMaxTBox16.Text = ""; StatRGMaxTBox17.Text = ""; StatRGMaxTBox18.Text = "0"; StatRGMaxTBox19.Text = "100"; StatRGMaxTBox20.Text = "0"; StatRGMaxTBox21.Text = "0"; StatRGMaxTBox22.Text = "0";
			SkillRGMaxTBox1.Text = ""; SkillRGMaxTBox2.Text = ""; SkillRGMaxTBox3.Text = ""; SkillRGMaxTBox4.Text = ""; SkillRGMaxTBox5.Text = ""; SkillRGMaxTBox6.Text = ""; SkillRGMaxTBox7.Text = ""; SkillRGMaxTBox8.Text = ""; SkillRGMaxTBox9.Text = ""; SkillRGMaxTBox10.Text = "";
			this.listBox_RGirlsList.SelectedIndexChanged -= new System.EventHandler(this.listBox_RGirlsList_SelectedIndexChanged);		//turns off SelectedIndexChanged handler, if this isn't used when we delete item at current index, new index would be CHANGED to -1, this would of course activate handler who would try to parse data at index -1, problem is, there is no data at -1 so this would produce an error, so it's best to simply turn this handler off while we delete stuff
			listBox_RGirlsList.SelectedItem = null;
			this.listBox_RGirlsList.SelectedIndexChanged += new System.EventHandler(this.listBox_RGirlsList_SelectedIndexChanged);		//turns SelectedIndexChanged handler back on
			checkBox_RGHuman.Checked = true;
		}

		//Stats normalized randomize, rounded to 10
		private void buttonRGNormRand1_Click(object sender, EventArgs e)
		{
			RandomRGNorm(StatRGMinTBox1, StatRGMaxTBox1, 1, 11, rnd, 10);	   //Charisma
			RandomRGNorm(StatRGMinTBox3, StatRGMaxTBox3, 1, 11, rnd, 10);	   //Libido
			RandomRGNorm(StatRGMinTBox4, StatRGMaxTBox4, 1, 11, rnd, 10);	   //Constitution
			RandomRGNorm(StatRGMinTBox5, StatRGMaxTBox5, 1, 11, rnd, 10);	   //Intelligence
			RandomRGNorm(StatRGMinTBox6, StatRGMaxTBox6, 1, 11, rnd, 10);	   //Confidence
			RandomRGNorm(StatRGMinTBox7, StatRGMaxTBox7, 1, 11, rnd, 10);	   //Mana
			RandomRGNorm(StatRGMinTBox8, StatRGMaxTBox8, 1, 11, rnd, 10);	   //Agility
			RandomRG(StatRGMinTBox14, StatRGMaxTBox14, 18, 101, rnd);		   //Age
			RandomRGNorm(StatRGMinTBox15, StatRGMaxTBox15, 1, 11, rnd, 10);	 //Obedience
			RandomRGNorm(StatRGMinTBox16, StatRGMaxTBox16, 1, 11, rnd, 10);	 //Spirit
			RandomRGNorm(StatRGMinTBox17, StatRGMaxTBox17, 1, 11, rnd, 10);	 //Beauty
		}

		//Skills normalized randomize, rounded to 5
		private void buttonRGNormRand2_Click(object sender, EventArgs e)
		{
			RandomRGNorm(SkillRGMinTBox1, SkillRGMaxTBox1, 0, 21, rnd, 5);		 //Anal Sex
			RandomRGNorm(SkillRGMinTBox2, SkillRGMaxTBox2, 0, 21, rnd, 5);		 //Magic Ability
			RandomRGNorm(SkillRGMinTBox3, SkillRGMaxTBox3, 0, 21, rnd, 5);		 //BDSM Sex
			RandomRGNorm(SkillRGMinTBox4, SkillRGMaxTBox4, 0, 21, rnd, 5);		 //Normal Sex
			RandomRGNorm(SkillRGMinTBox5, SkillRGMaxTBox5, 0, 21, rnd, 5);		 //Bestiality Sex
			RandomRGNorm(SkillRGMinTBox6, SkillRGMaxTBox6, 0, 21, rnd, 5);		 //Group Sex
			RandomRGNorm(SkillRGMinTBox7, SkillRGMaxTBox7, 0, 21, rnd, 5);		 //Lesbian Sex
			RandomRGNorm(SkillRGMinTBox8, SkillRGMaxTBox8, 0, 21, rnd, 5);		 //Service Skills
			RandomRGNorm(SkillRGMinTBox9, SkillRGMaxTBox9, 0, 21, rnd, 5);		 //Stripping Sex
			RandomRGNorm(SkillRGMinTBox10, SkillRGMaxTBox10, 0, 21, rnd, 5);	   //Combat Ability
		}

		//Stats conditional randomize
		private void buttonRGCondRand1_Click(object sender, EventArgs e)
		{
			RandomRGNorm(StatRGMinTBox1, StatRGMaxTBox1, 1, 6, rnd, 10);		//Charisma

			RandomRGNorm(StatRGMinTBox3, StatRGMaxTBox3, 1, 7, rnd, 10);		//Libido

			//check if some of "constitution" traits are selected, if they are constitution has chance to get higher than without them

			bool bStats = false;
			for (int i = 0; i < rgTable.Rows.Count; i++)
			{
				if (rgTable.Rows[i][0].ToString() == "Adventurer" || rgTable.Rows[i][0].ToString() == "Assassin" || rgTable.Rows[i][0].ToString() == "Cool Scars")
				{
					bStats = true;
					break;
				}
			}

			if (bStats == true) RandomRGNorm(StatRGMinTBox4, StatRGMaxTBox4, 5, 10, rnd, 10);
			else RandomRGNorm(StatRGMinTBox4, StatRGMaxTBox4, 3, 6, rnd, 10);

			RandomRGNorm(StatRGMinTBox5, StatRGMaxTBox5, 3, 10, rnd, 10);	   //Intelligence
			RandomRGNorm(StatRGMinTBox6, StatRGMaxTBox6, 2, 9, rnd, 10);		//Confidence
			RandomRGNorm(StatRGMinTBox7, StatRGMaxTBox7, 0, 4, rnd, 10);		//Mana
			RandomRGNorm(StatRGMinTBox8, StatRGMaxTBox8, 3, 10, rnd, 10);	   //Agility
			RandomRG(StatRGMinTBox14, StatRGMaxTBox14, 18, 71, rnd);			//Age
			RandomRGNorm(StatRGMinTBox15, StatRGMaxTBox15, 1, 8, rnd, 10);	  //Obedience
			RandomRGNorm(StatRGMinTBox16, StatRGMaxTBox16, 2, 8, rnd, 10);	  //Spirit
			RandomRGNorm(StatRGMinTBox17, StatRGMaxTBox17, 3, 8, rnd, 10);	  //Beauty
		}

		//Skills conditional randomize
		private void buttonRGCondRand2_Click(object sender, EventArgs e)
		{
			//again as with constitution, if she has Strong Magic trait, she'll have higher chance of having higher Magic Ability
			bool bStats = false;
			for (int i = 0; i < rgTable.Rows.Count; i++)
			{
				if (rgTable.Rows[i][0].ToString() == "Strong Magic")
				{
					bStats = true;
					break;
				}
			}


			if (bStats == true) RandomRGNorm(SkillRGMinTBox2, SkillRGMaxTBox2, 8, 16, rnd, 5);
			else RandomRGNorm(SkillRGMinTBox2, SkillRGMaxTBox2, 0, 9, rnd, 5);

			//Combat ability check
			bStats = false;
			for (int i = 0; i < rgTable.Rows.Count; i++)
			{
				if (rgTable.Rows[i][0].ToString() == "Adventurer" || rgTable.Rows[i][0].ToString() == "Assassin" || rgTable.Rows[i][0].ToString() == "Cool Scars")
				{
					bStats = true;
					break;
				}
			}

			if (bStats == true) RandomRGNorm(SkillRGMinTBox10, SkillRGMaxTBox10, 8, 16, rnd, 5);
			else RandomRGNorm(SkillRGMinTBox10, SkillRGMaxTBox10, 0, 9, rnd, 5);

			RandomRGNorm(SkillRGMinTBox1, SkillRGMaxTBox1, 0, 9, rnd, 5);	   //Anal Sex
			RandomRGNorm(SkillRGMinTBox3, SkillRGMaxTBox3, 0, 9, rnd, 5);	   //BDSM Sex
			RandomRGNorm(SkillRGMinTBox4, SkillRGMaxTBox4, 0, 9, rnd, 5);	   //Normal Sex
			RandomRGNorm(SkillRGMinTBox5, SkillRGMaxTBox5, 0, 9, rnd, 5);	   //Bestiality Sex
			RandomRGNorm(SkillRGMinTBox6, SkillRGMaxTBox6, 0, 9, rnd, 5);	   //Group Sex
			RandomRGNorm(SkillRGMinTBox7, SkillRGMaxTBox7, 0, 9, rnd, 5);	   //Lesbian Sex
			RandomRGNorm(SkillRGMinTBox8, SkillRGMaxTBox8, 0, 9, rnd, 5);	   //Service Skill
			RandomRGNorm(SkillRGMinTBox9, SkillRGMaxTBox9, 0, 9, rnd, 5);	   //Stripping Skill

		}

		//button that loads existing random girls file
		private void button_LoadRGirls_Click(object sender, EventArgs e)
		{
			OpenFileDialog Open = new OpenFileDialog();
			Open.Filter = "Whore Master random girls file|*.rgirlsx;*.rgirls|All files|*.*";
			Open.ShowDialog();

			switch (Path.GetExtension(Open.FileName))
			{
				case ".rgirlsx":
					LoadRGirlsXML(Open.FileName);
					break;
				case ".rgirls":
					LoadRGirls(Open.FileName);
					break;
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
					string line9 = Convert.ToString(Import.ReadLine());		 //is human flag (0 for human, 1 for no human
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
			catch //(Exception err)
			{
				//MessageBox.Show(err.Message, "Open error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
					string sName = node.Attributes["Name"].Value;				   //girl name
					string sDesc = node.Attributes["Desc"].Value;				   //girl description

					string sHuman = "";											 //human or not human flag

					switch (node.Attributes["Human"].Value)
					{
						case "Yes":
							sHuman = "0";
							break;
						case "No":
							sHuman = "1";
							break;
					}

					string sCatacombs = "";

					switch (node.Attributes["Catacomb"].Value)
					{
						case "Yes":
							sCatacombs = "1";
							break;
						case "No":
							sCatacombs = "0";
							break;
					}

					string sMinMoney = node["Gold"].Attributes["Min"].Value; 
					string sMaxMoney = node["Gold"].Attributes["Max"].Value;

					string[] aCharisma = new string[2]; string[] aHappiness = new string[2]; string[] aLibido = new string[2]; string[] aConstitution = new string[2]; string[] aIntelligence = new string[2]; string[] aConfidence = new string[2]; string[] aMana = new string[2]; string[] aAgility = new string[2]; string[] aFame = new string[2]; string[] aLevel = new string[2];
					string[] aAskPrice = new string[2]; string[] aHouse = new string[2]; string[] aExp = new string[2]; string[] aAge = new string[2]; string[] aObedience = new string[2]; string[] aSpirit = new string[2]; string[] aBeauty = new string[2]; string[] aTiredness = new string[2]; string[] aHealth = new string[2]; string[] aPCFear = new string[2];
					string[] aPCLove = new string[2]; string[] aPCHate = new string[2];

					string[] aAnal = new string[2]; string[] aMagic = new string[2]; string[] aBDSM = new string[2]; string[] aNormalSex = new string[2]; string[] aBeastiality = new string[2];
					string[] aGroup = new string[2]; string[] aLesbian = new string[2]; string[] aService = new string[2]; string[] aStrip = new string[2]; string[] aCombat = new string[2];

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

					string sMinStat = aCharisma[0] + " " + aHappiness[0] + " " + aLibido[0] + " " + aConstitution[0] + " " + aIntelligence[0] + " " + aConfidence[0] + " " + aMana[0] + " " + aAgility[0] + " " + aFame[0] + " " + aLevel[0] + " " + aAskPrice[0] + " " + aHouse[0] + " " + aExp[0] + " " + aAge[0] + " " + aObedience[0] + " " + aSpirit[0] + " " + aBeauty[0] + " " + aTiredness[0] + " " + aHealth[0] + " " + aPCFear[0] + " " + aPCLove[0] + " " + aPCHate[0];
					string sMaxStat = aCharisma[1] + " " + aHappiness[1] + " " + aLibido[1] + " " + aConstitution[1] + " " + aIntelligence[1] + " " + aConfidence[1] + " " + aMana[1] + " " + aAgility[1] + " " + aFame[1] + " " + aLevel[1] + " " + aAskPrice[1] + " " + aHouse[1] + " " + aExp[1] + " " + aAge[1] + " " + aObedience[1] + " " + aSpirit[1] + " " + aBeauty[1] + " " + aTiredness[1] + " " + aHealth[1] + " " + aPCFear[1] + " " + aPCLove[1] + " " + aPCHate[1];

					string sMinSkill = aAnal[0] + " " + aMagic[0] + " " + aBDSM[0] + " " + aNormalSex[0] + " " + aBeastiality[0] + " " + aGroup[0] + " " + aLesbian[0] + " " + aService[0] + " " + aStrip[0] + " " + aCombat[0];
					string sMaxSkill = aAnal[1] + " " + aMagic[1] + " " + aBDSM[1] + " " + aNormalSex[1] + " " + aBeastiality[1] + " " + aGroup[1] + " " + aLesbian[1] + " " + aService[1] + " " + aStrip[1] + " " + aCombat[1];

					string sTraits = tnum.ToString();

					for (int y = 0; y < tnum;y++)
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
				MessageBox.Show(err.Message, "XML load error error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
				MessageBox.Show("It's probably nothing serious, i.e. you pressed Delete button\n\rwhile list was empty or nothing was selected, now for \"official\" error:\n\r\n\r" + err.Message, "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				StatusLabel1.Text = "Delete item error...";
			}
		}

		//button that compiles random girls data back from dataTable to output tab
		private void button_SaveRGirls_Click(object sender, EventArgs e)
		{
			Filesave.FileName = "";
			Filesave.Filter = "Whore Master random girls XML file|*.rgirlsx|All files|*.*";
			Filesave.ShowDialog();

			try
			{
				if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
				if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");
				SaveRGirlsXML(Filesave.FileName);
				StatusLabel1.Text = "Successfully compiled " + listBox_RGirlsList.Items.Count.ToString() + " random girls...";
			}
			catch
			{
			}

/*			
			Filesave.FileName = "";
			Filesave.Filter = "Whore Master random girls XML file|*.rgirlsx|Whore Master random girls file|*.rgirls|All files|*.*";

			try
			{
				Filesave.ShowDialog();
				if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
				if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");

				switch (Path.GetExtension(Filesave.FileName))
				{
					case ".rgirls":
						SaveRGirls(Filesave.FileName);
						break;
					case ".rgirlsx":
						SaveRGirlsXML(Filesave.FileName);
						break;
				}
			}
			catch
			{
			}
			StatusLabel1.Text = "Successfully compiled " + listBox_RGirlsList.Items.Count.ToString() + " random girls...";
*/
		}

		//save girls function
		private void SaveRGirls(string path)
		{
			string output = "";
			for (int i = 0; i < listBox_RGirlsList.Items.Count; i++)
			{
				if (i +1 == listBox_RGirlsList.Items.Count) output = output + RGirlsCollection.Rows[i][0] + "\r\n" + RGirlsCollection.Rows[i][1];
				else output = output + RGirlsCollection.Rows[i][0] + "\r\n" + RGirlsCollection.Rows[i][1] + "\r\n";
			}

			StreamWriter Export = new StreamWriter(Convert.ToString(path));
			Export.Write(output);
			Export.Close();
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

			string[] sStats = new string[22] { "Charisma", "Happiness", "Libido", "Constitution", "Intelligence", "Confidence", "Mana", "Agility", "Fame", "Level", "AskPrice", "House", "Exp", "Age", "Obedience", "Spirit", "Beauty", "Tiredness", "Health", "PCFear", "PCLove", "PCHate" };
			string[] sSkills = new string[10] { "Anal", "Magic", "BDSM", "NormalSex", "Beastiality", "Group", "Lesbian", "Service", "Strip", "Combat" };

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
					case "0":
						sHuman = "Yes";
						break;
					case "1":
						sHuman = "No";
						break;
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
					stat.SetAttribute("Name", sStats[y]);
					stat.SetAttribute("Min", sMinStat[y]);
					stat.SetAttribute("Max", sMaxStat[y]);
					girl.AppendChild(stat);
				}

				for (int y = 0; y < sSkills.Count(); y++)
				{
					skill = xmldoc.CreateElement("Skill");
					skill.SetAttribute("Name", sSkills[y]);
					skill.SetAttribute("Min", sMinSkill[y]);
					skill.SetAttribute("Max", sMaxSkill[y]);
					girl.AppendChild(skill);
				}

				string sCatacombs = "";

				switch (sData.ReadLine())
				{
					case "0":
						sCatacombs = "No";
						break;
					case "1":
						sCatacombs = "Yes";
						break;
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
			settings.NewLineOnAttributes = true;
			settings.IndentChars = "\t";
			XmlWriter xmlwrite = XmlWriter.Create(path, settings);

			xmldoc.Save(xmlwrite);
			xmlwrite.Close();
		}

		//clears random girls list
		private void button_RGirlsClearList_Click(object sender, EventArgs e)
		{
			RGirlsCollection.Clear();
			listBox_RGirlsList.Items.Clear();
			StatusLabel1.Text = "Random girl list cleared...";
		}

		//parses selected random girl to enable editing
		private void listBox_RGirlsList_SelectedIndexChanged(object sender, EventArgs e)
		{
			nameRTBox1.Text = RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][0].ToString();
			string sData = RGirlsCollection.Rows[listBox_RGirlsList.SelectedIndex][1].ToString();

			StringReader data = new StringReader(sData);
			descRTBox1.Text = data.ReadLine();


			char[] separator = { ' ' };											 //data in item data is space delimited, so to begin extraction from it we create an array of separator characters
			string[] values = data.ReadLine().Split(separator);					 //use Split function to put every value that's "between" spaces to array
			StatRGMinTBox1.Text = values[0]; StatRGMinTBox2.Text = values[1]; StatRGMinTBox3.Text = values[2]; StatRGMinTBox4.Text = values[3]; StatRGMinTBox5.Text = values[4]; StatRGMinTBox6.Text = values[5]; StatRGMinTBox7.Text = values[6]; StatRGMinTBox8.Text = values[7]; StatRGMinTBox9.Text = values[8]; StatRGMinTBox10.Text = values[9]; StatRGMinTBox11.Text = values[10]; StatRGMinTBox12.Text = values[11]; StatRGMinTBox13.Text = values[12]; StatRGMinTBox14.Text = values[13]; StatRGMinTBox15.Text = values[14]; StatRGMinTBox16.Text = values[15]; StatRGMinTBox17.Text = values[16]; StatRGMinTBox18.Text = values[17]; StatRGMinTBox19.Text = values[18]; StatRGMinTBox20.Text = values[19]; StatRGMinTBox21.Text = values[20]; StatRGMinTBox22.Text = values[21];

			values = data.ReadLine().Split(separator);
			StatRGMaxTBox1.Text = values[0]; StatRGMaxTBox2.Text = values[1]; StatRGMaxTBox3.Text = values[2]; StatRGMaxTBox4.Text = values[3]; StatRGMaxTBox5.Text = values[4]; StatRGMaxTBox6.Text = values[5]; StatRGMaxTBox7.Text = values[6]; StatRGMaxTBox8.Text = values[7]; StatRGMaxTBox9.Text = values[8]; StatRGMaxTBox10.Text = values[9]; StatRGMaxTBox11.Text = values[10]; StatRGMaxTBox12.Text = values[11]; StatRGMaxTBox13.Text = values[12]; StatRGMaxTBox14.Text = values[13]; StatRGMaxTBox15.Text = values[14]; StatRGMaxTBox16.Text = values[15]; StatRGMaxTBox17.Text = values[16]; StatRGMaxTBox18.Text = values[17]; StatRGMaxTBox19.Text = values[18]; StatRGMaxTBox20.Text = values[19]; StatRGMaxTBox21.Text = values[20]; StatRGMaxTBox22.Text = values[21];

			values = data.ReadLine().Split(separator);
			SkillRGMinTBox1.Text = values[0]; SkillRGMinTBox2.Text = values[1]; SkillRGMinTBox3.Text = values[2]; SkillRGMinTBox4.Text = values[3]; SkillRGMinTBox5.Text = values[4]; SkillRGMinTBox6.Text = values[5]; SkillRGMinTBox7.Text = values[6]; SkillRGMinTBox8.Text = values[7]; SkillRGMinTBox9.Text = values[8]; SkillRGMinTBox10.Text = values[9];

			values = data.ReadLine().Split(separator);
			SkillRGMaxTBox1.Text = values[0]; SkillRGMaxTBox2.Text = values[1]; SkillRGMaxTBox3.Text = values[2]; SkillRGMaxTBox4.Text = values[3]; SkillRGMaxTBox5.Text = values[4]; SkillRGMaxTBox6.Text = values[5]; SkillRGMaxTBox7.Text = values[6]; SkillRGMaxTBox8.Text = values[7]; SkillRGMaxTBox9.Text = values[8]; SkillRGMaxTBox10.Text = values[9];

			GoldRMinTBox1.Text = data.ReadLine(); GoldRMaxTBox1.Text = data.ReadLine();

			string line9 = data.ReadLine();										  //"is human" flag
			switch (line9)
			{
				case "0":
					checkBox_RGHuman.Checked = true;
					break;
				case "1":
					checkBox_RGHuman.Checked = false;
					break;
				default:
					checkBox_RGHuman.Checked = true;
					break;
			}

			string line10 = data.ReadLine();														//this line "is" always 0, or at least that's what manual says, but is seems this is catacomb flag, although it doesn't do anything atm
			switch (line10)
			{
				case "0":
					checkBox_RGCatacombs.Checked = false;
					break;
				case "1":
					checkBox_RGCatacombs.Checked = true;
					break;
				default:
					checkBox_RGCatacombs.Checked = false;
					break;
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
			ListBox listBox = (ListBox)sender;								  //creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
			int index = listBox.IndexFromPoint(e.Location);					 //e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
			if (index > -1 && index < listBox.Items.Count)					  //if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
			{
				string tip = listBox.Items[index].ToString();				   //this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
				if (tip != lastTip)											 //every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
				{															   //also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
					toolTip1.SetToolTip(listBox, new StringReader(RGirlsCollection.Rows[index][1].ToString()).ReadLine());	//this is the only difference from previous tooltip function. Description is stored in dataTable at [index][1], along with other data, so to get it out we first need to identify coordinates (that's what [index][1] is for), then construct StringReader from that long string, and lastly read first line from that stream, this is description. Luckily it can be fitted in one line :P
					lastTip = tip;											  //updates lastTip string
				}
			}
		}

		//*************************
		//*****   Items tab   *****
		//*************************

		private void trackBar1_Scroll(object sender, EventArgs e)
		{
			lTrack1.Text = trackBar_bad_02.Value.ToString();
		}

		//Since some drop boxes have different contents in regard of previus one this is how to do it. Simply if index is this and this, clear droplist and fill these in
		//also, in status case there's no value, it's simply add or remove, so in case status is selected value textbox is hidden and add/remove droplist is displayed, and vice versa
		private void comboBox_affects_01_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (comboBox_affects_01.SelectedIndex == 0)			 //Skills
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
				affects_textBox_value.Text = "";
			}
			if (comboBox_affects_01.SelectedIndex == 1)			 //Stats
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
				affects_textBox_value.Text = "";
			}
			if (comboBox_affects_01.SelectedIndex == 2)			 //Status
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
			}

			//not all combinations are possible, this takes care of that, duration is disabled in these cases

			if ((comboBox_affects_01.SelectedIndex == 0 && comboBox_ItemType_01.SelectedIndex == 3) || (comboBox_affects_01.SelectedIndex == 1 && comboBox_ItemType_01.SelectedIndex == 3))		 //if consumables and skills, or consumables and stats are selected duration drop list is enabled
			{
				comboBox_ItemDuration.Enabled = true;			   //enables duration drop list in case it was disabled
			}
			else													//for other combinations (either item is not consumable, or it is consumable, but status is selected disable it and reset it's value
			{
				comboBox_ItemDuration.Enabled = false;				  //item duration doesn't affect status changes so to be better safe than sorry we disable it and set it do default value
				comboBox_ItemDuration.SelectedIndex = 0;
			}

		}

		//adds special effects to the item datagrid
		private void button_AddToItemDataGrid_Click(object sender, EventArgs e)
		{
			try
			{
				if (comboBox_affects_01.SelectedIndex == 2) iTable.Rows.Add(comboBox_affects_01.SelectedItem.ToString(), comboBox_affects_02.SelectedItem.ToString(), affects_comboBox_status.SelectedItem.ToString());
				else iTable.Rows.Add(comboBox_affects_01.SelectedItem.ToString(), comboBox_affects_02.SelectedItem.ToString(), affects_textBox_value.Text);
			}
			catch
			{
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
			catch
			{
				MessageBox.Show("Nothing is selected or list is empty.", "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			}
		}

		//adds trait to item
		private void button_AddTraitsToItem_Click(object sender, EventArgs e)
		{
			try
			{
				iTTable.Rows.Add(comboBox_ItemTraits.SelectedItem.ToString(), comboBox_ItemTraitsEffect.SelectedItem.ToString());
			}
			catch
			{
				MessageBox.Show("One or more fields are empty", "Add error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			}
		}

		//remove selected trait
		private void button_RemoveTraitsFromItem_Click(object sender, EventArgs e)
		{
			try
			{
				iTTable.Rows[dataGridView3.CurrentRow.Index].Delete();
			}
			catch
			{
				MessageBox.Show("Nothing is selected or list is empty.", "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
			}
		}

		//method that compiles values from items tab to game format
		private string[] CompileItem(string output)
		{
			string[] sItem = { "", "" };			   //string array that will be returned, as usual, name at 0 index, data as 1 index
			string nln = "\r\n";
			string sp = " ";
			string sItemInfinite = "";			  //item is infinite in shop?
			string sEffectNumber = "";			  //number of effects added, and also traits added, no need for separate variable since it's used only once in each case
			string sEffects = "";				   //string to store effects added when done
			string sTraits = "";					//string to store traits added when done
			string sDuration = "0";				 //trait duration 

			if (checkBox_infinite_06.Checked == true) sItemInfinite = "1";
			else sItemInfinite = "0";

			if (dataGridView2.RowCount == 0) sEffectNumber = "0";				   //read item effects from datagrid and parse them to game format
			else
			{
				sEffectNumber = dataGridView2.RowCount.ToString();
				int i = 0;
				while (i < dataGridView2.RowCount)
				{
					sEffects = sEffects + nln + EffectParse(dataGridView2.Rows[i].Cells[0].Value.ToString(), dataGridView2.Rows[i].Cells[1].Value.ToString(), dataGridView2.Rows[i].Cells[2].Value.ToString());
					i = i + 1;
				}
			}

			if (dataGridView3.RowCount > 0)										 //read item traits from datagrid and parse them to game format
			{
				sEffectNumber = (Convert.ToInt32(sEffectNumber) + dataGridView3.RowCount).ToString();
				int i = 0;
				while (i < dataGridView3.RowCount)
				{
					sTraits = sTraits + nln + TraitsParse(dataGridView3.Rows[i].Cells[0].Value.ToString(), dataGridView3.Rows[i].Cells[1].Value.ToString());
					i = i + 1;
				}
			}

			if (comboBox_ItemDuration.SelectedIndex == 0) sDuration = "0";		  //sets flag 3 to Default behaviour (permanent)
			if (comboBox_ItemDuration.SelectedIndex == 1) sDuration = "1";		  //sets flag 3 to Affects all girls temporarily
			if (comboBox_ItemDuration.SelectedIndex == 2) sDuration = "2";		  //sets flag 3 to Affects one girl temporarily

			if (output == "single") sItem[0] = textBox_ItemName.Text + nln + textBox_ItemDesc.Text + nln + (comboBox_ItemType_01.SelectedIndex + 1).ToString() + sp + trackBar_bad_02.Value.ToString() + sp + sDuration + sp + textBox_itemcost_04.Text + sp + comboBox_Rarity_05.SelectedIndex.ToString() + sp + sItemInfinite + nln + sEffectNumber + sEffects + sTraits;  //not sure what (if anything) this does, it seems that this is obsolete now as compile button passes only "list" parameter, or better said, in current app this doesn't do anything, but I left it here "just in case"
			else if (output == "list")
			{
				sItem[0] = textBox_ItemName.Text;

				if (checkBox_Item_GirlBuyChance.Checked == true) sItem[1] = textBox_ItemDesc.Text + nln + (comboBox_ItemType_01.SelectedIndex + 1).ToString() + sp + trackBar_bad_02.Value.ToString() + sp + sDuration + sp + textBox_itemcost_04.Text + sp + comboBox_Rarity_05.SelectedIndex.ToString() + sp + sItemInfinite + sp + textBox_Item_GirlBuyChance.Text + nln + sEffectNumber + sEffects + sTraits;
				else sItem[1] = textBox_ItemDesc.Text + nln + (comboBox_ItemType_01.SelectedIndex + 1).ToString() + sp + trackBar_bad_02.Value.ToString() + sp + sDuration + sp + textBox_itemcost_04.Text + sp + comboBox_Rarity_05.SelectedIndex.ToString() + sp + sItemInfinite + nln + sEffectNumber + sEffects + sTraits;
			}

			return sItem;
		}

		//adds item to item list (DataTable and ComboBox)
		private void button12_Click(object sender, EventArgs e)
		{
			ItemsCollection.Rows.Add(CompileItem("list").ElementAt(0), CompileItem("list").ElementAt(1));
			SortDataTable(ref ItemsCollection, ref listBox_ItemsList, textBox_ItemName.Text);
			StatusLabel1.Text = "Added item " + "\"" + ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0] + "\"" + " to list...";
		}

		//replaces values at currently selected index in DataTable and ComboBox
		private void button13_Click(object sender, EventArgs e)
		{
			ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0] = CompileItem("list").ElementAt(0);
			ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][1] = CompileItem("list").ElementAt(1);
			SortDataTable(ref ItemsCollection, ref listBox_ItemsList, textBox_ItemName.Text);
			StatusLabel1.Text = "Changed item entry No" + (listBox_ItemsList.SelectedIndex + 1).ToString() + " " + "(\"" + ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0] + "\")...";
		}

		//resets items tab
		private void button14_Click(object sender, EventArgs e)
		{
			textBox_ItemName.Text = "";
			textBox_ItemDesc.Text = "";
			comboBox_ItemType_01.SelectedIndex = 3;
			comboBox_Rarity_05.SelectedIndex = 0;
			comboBox_ItemDuration.SelectedIndex = 0;
			textBox_itemcost_04.Text = "10";
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
			string sEf01 = "";	  //type
			string sEf02 = "";	  //attribute
			string sEf03 = "";	  //value

			string[,] aTypes = new string[3, 2] { { "Skill", "0" }, { "Stat", "1" }, { "Status", "3" } };
			string[,] aSkills = new string[10, 2] { { "Anal", "0" }, { "Magic", "1" }, { "BDSM", "2" }, { "Normal Sex", "3" }, { "Bestiality", "4" }, { "Group", "5" }, { "Lesbian", "6" }, { "Service", "7" }, { "Strip", "8" }, { "Combat", "9" } };
			string[,] aStats = new string[22, 2] { { "Charisma", "0" }, { "Happiness", "1" }, { "Libedo", "2" }, { "Constitution", "3" }, { "Intelligence", "4" }, { "Confidence", "5" }, { "Mana", "6" }, { "Agility", "7" }, { "Fame", "8" }, { "Level", "9" }, { "AskPrice", "10" }, { "House", "11" }, { "Experience", "12" }, { "Age", "13" }, { "Obedience", "14" }, { "Spirit", "15" }, { "Beauty", "16" }, { "Tiredness", "17" }, { "Health", "18" }, { "PC Fear", "19" }, { "PC Love", "20" }, { "PC Hate", "21" } };
			string[,] aStatus = new string[10, 2] { { "Poisoned", "1" }, { "Badly Poisoned", "2" }, { "Pregnant", "3" }, { "Pregnant By Player", "4" }, { "Slave", "5" }, { "Has daughter", "6" }, { "Has son", "7" }, { "Inseminated", "8" }, { "Controlled", "9" }, { "Catacombs", "10" } };

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
				else i = i + 1;
			}

			//now, depending on what type it is compare against it's respective array and read value linked to that attribute

			if (sEf01 == "0")
			{
				i = 0;
				while (i < 10)
				{
					if (attribute == aSkills[i, 0])
					{
						sEf02 = aSkills[i, 1];
						break;
					}
					else i = i + 1;
				}
			}
			else if (sEf01 == "1")
			{
				i = 0;
				while (i < 22)
				{
					if (attribute == aStats[i, 0])
					{
						sEf02 = aStats[i, 1];
						break;
					}
					else i = i + 1;
				}
			}
			else if (sEf01 == "3")
			{
				i = 0;
				while (i < 9)
				{
					if (attribute == aStatus[i, 0])
					{
						sEf02 = aStatus[i, 1];
						break;
					}
					else i = i + 1;
				}
			}

			//these do not need arrays, for skill and stat value is already entered, for status we just need to check if it's Add or not and "act" accordingly

			if (sEf01 == "0" || sEf01 == "1") sEf03 = value;
			else if (sEf01 == "3")
			{
				if (value == "Add") sEf03 = "1";
				else sEf03 = "0";
			}

			return (sEf01 + " " + sEf02 + " " + sEf03);		 //and in the end return finished effect in game format (number number number)
		}

		//parses traits to game values
		private string TraitsParse(string trait, string value)
		{
			//traits are much simpler than items, they're represented by their names so there's no need to assign numbers, just take the trait and check if it's add or remove and apply that

			string sTr = "";

			if (value == "Add") sTr = "1";
			else sTr = "0";

			return ("4" + "\r\n" + trait + "\r\n" + sTr);
		}

		//toggles readonly attribute for value field in datagridview
		private void checkBox_DG2_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox_DG2.Checked == true) dataGridView2.Columns[2].ReadOnly = true;
			else dataGridView2.Columns[2].ReadOnly = false;
		}

		//toggles readonly attribute for value field in datagridview	
		private void checkBox_DG3_CheckedChanged(object sender, EventArgs e)
		{
			if (checkBox_DG3.Checked == true) dataGridView3.Columns[1].ReadOnly = true;
			else dataGridView3.Columns[1].ReadOnly = false;
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
			catch /*(Exception err)*/
			{
				//MessageBox.Show(err.Message, "Open error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
					string sName = node.Attributes["Name"].Value;				   //item name
					string sDesc = node.Attributes["Desc"].Value;				   //item description
					string sData = "";											  //string that will store item data
					ArrayList alEffects = new ArrayList();						  //ArrayList to store parsed effects

					switch (node.Attributes["Type"].Value)						  //testing for item type, 1st number in "old" format
					{
						case "Ring":
							sData = "1";
							break;
						case "Dress":
							sData = "2";
							break;
						case "Shoes":
							sData = "3";
							break;
						case "Food":
							sData = "4";
							break;
						case "Necklace":
							sData = "5";
							break;
						case "Weapon":
							sData = "6";
							break;
						case "Makeup":
							sData = "7";
							break;
						case "Armor":
							sData = "8";
							break;
						case "Misc":
							sData = "9";
							break;
						case "Armband":
							sData = "10";
							break;
						case "Small Weapon":
							sData = "11";
							break;
					}

					sData = sData + " " + node.Attributes["Badness"].Value;		 //item "badness" value, 2nd value

					switch (node.Attributes["Special"].Value)					   //item special value, how it effects girl(s), 3nd value
					{
						case "None":
							sData = sData + " " + "0";
							break;
						case "AffectsAll":
							sData = sData + " " + "1";
							break;
						case "Temporary":
							sData = sData + " " + "2";
							break;
					}

					sData = sData + " " + node.Attributes["Cost"].Value;			//item cost, 4th value

					switch (node.Attributes["Rarity"].Value)						//item rarity, 5th value
					{
						case "Common":
							sData = sData + " " + "0";
							break;
						case "Shop50":
							sData = sData + " " + "1";
							break;
						case "Shop25":
							sData = sData + " " + "2";
							break;
						case "Shop05":
							sData = sData + " " + "3";
							break;
						case "Catacomb15":
							sData = sData + " " + "4";
							break;
						case "ScriptOnly":
							sData = sData + " " + "5";
							break;
						case "ScriptOrReward":
							sData = sData + " " + "6";
							break;
					}

					switch (node.Attributes["Infinite"].Value)					  //item is infinite, 6th value
					{
						case "true":
							sData = sData + " " + "1";
							break;
						case "false":
							sData = sData + " " + "0";
							break;
					}

					if (node.Attributes["GirlBuyChance"] != null)				   //this will be 7th vallue, it doesn't exist in original items
					{
						sData = sData + " " + node.Attributes["GirlBuyChance"].Value;
					}

					if (node.HasChildNodes == true)
					{
						for (int x = 0; x < node.ChildNodes.Count; x++)
						{
							string sEffect = "";

							switch (node.ChildNodes[x].Attributes["What"].Value)	//what item affects, 1st of 3 numbers that define items
							{
								case "Skill":
									sEffect = "0";
									switch (node.ChildNodes[x].Attributes["Name"].Value)		//2nd number, in this case what skill it affects, I have to nest them since each of the types has different choices (i.e. anal as skill and intelligence as stat) so they won't get mixed
									{
										case "Anal":
											sEffect = sEffect + " " + "0";
											break;
										case "Magic":
											sEffect = sEffect + " " + "1";
											break;
										case "BDSM":
											sEffect = sEffect + " " + "2";
											break;
										case "NormalSex":
											sEffect = sEffect + " " + "3";
											break;
										case "Beastiality":
											sEffect = sEffect + " " + "4";
											break;
										case "Group":
											sEffect = sEffect + " " + "5";
											break;
										case "Lesbian":
											sEffect = sEffect + " " + "6";
											break;
										case "Service":
											sEffect = sEffect + " " + "7";
											break;
										case "Strip":
											sEffect = sEffect + " " + "8";
											break;
										case "Combat":
											sEffect = sEffect + " " + "9";
											break;
									}
									sEffect = sEffect + " " + node.ChildNodes[x].Attributes["Amount"].Value;
									break;
								case "Stat":
									sEffect = "1";
									switch (node.ChildNodes[x].Attributes["Name"].Value)
									{
										case "Charisma":
											sEffect = sEffect + " " + "0";
											break;
										case "Happiness":
											sEffect = sEffect + " " + "1";
											break;
										case "Libido":
											sEffect = sEffect + " " + "2";
											break;
										case "Constitution":
											sEffect = sEffect + " " + "3";
											break;
										case "Intelligence":
											sEffect = sEffect + " " + "4";
											break;
										case "Confidence":
											sEffect = sEffect + " " + "5";
											break;
										case "Mana":
											sEffect = sEffect + " " + "6";
											break;
										case "Agility":
											sEffect = sEffect + " " + "7";
											break;
										case "Fame":
											sEffect = sEffect + " " + "8";
											break;
										case "Level":
											sEffect = sEffect + " " + "9";
											break;
										case "AskPrice":
											sEffect = sEffect + " " + "10";
											break;
										case "House":
											sEffect = sEffect + " " + "11";
											break;
										case "Exp":
											sEffect = sEffect + " " + "12";
											break;
										case "Age":
											sEffect = sEffect + " " + "13";
											break;
										case "Obedience":
											sEffect = sEffect + " " + "14";
											break;
										case "Spirit":
											sEffect = sEffect + " " + "15";
											break;
										case "Beauty":
											sEffect = sEffect + " " + "16";
											break;
										case "Tiredness":
											sEffect = sEffect + " " + "17";
											break;
										case "Health":
											sEffect = sEffect + " " + "18";
											break;
										case "PCFear":
											sEffect = sEffect + " " + "19";
											break;
										case "PCLove":
											sEffect = sEffect + " " + "20";
											break;
										case "PCHate":
											sEffect = sEffect + " " + "21";
											break;

											//cases after this will just be for my errors, I should have thought of this sooner, they'll recognize errors in files and warn user to resave them to correct it
										case "PChate":						  //this one was a typo, so to enable people to load these files and to resave them
											sEffect = sEffect + " " + "21";
											MessageBox.Show("Item \"" + sName + "\"has a typo in it's \"PC Hate\" string,\r\nto fix it just resave the file and it will be saved correctly.", "Warning", MessageBoxButtons.OK, MessageBoxIcon.Information);
											break;
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
										case "Poisoned":
											sEffect = sEffect + " " + "1";
											break;
										case "Badly Poisoned":
											sEffect = sEffect + " " + "2";
											break;
										case "Pregnant":
											sEffect = sEffect + " " + "3";
											break;
										case "Pregnant By Player":
											sEffect = sEffect + " " + "4";
											break;
										case "Slave":
											sEffect = sEffect + " " + "5";
											break;
										case "Has Daughter":
											sEffect = sEffect + " " + "6";
											break;
										case "Has Son":
											sEffect = sEffect + " " + "7";
											break;
										case "Inseminated":
											sEffect = sEffect + " " + "8";
											break;
										case "Controlled":
											sEffect = sEffect + " " + "9";
											break;
										case "Catacombs":
											sEffect = sEffect + " " + "10";
											break;
									}
									sEffect = sEffect + " " + node.ChildNodes[x].Attributes["Amount"].Value;
									break;
								case "Trait":
									sEffect = "4" + "\r\n" + node.ChildNodes[x].Attributes["Name"].Value + "\r\n" + node.ChildNodes[x].Attributes["Amount"].Value;
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
				MessageBox.Show(err.Message, "XML load error error", MessageBoxButtons.OK, MessageBoxIcon.Error);
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
				MessageBox.Show("It's probably nothing serious, i.e. you pressed Delete button\n\rwhile list was empty or nothing was selected, now for \"official\" error:\n\r\n\r" + err.Message, "Delete error", MessageBoxButtons.OK, MessageBoxIcon.Error);
				StatusLabel1.Text = "Delete item error...";
			}
		}

		//this happens when you select something from item list listbox, that invokes SelectedIndexChanged event and values from entry with this index get filled to item tab boxes
		private void listBox_ItemsList_SelectedIndexChanged(object sender, EventArgs e)
		{
			string sEf01 = "";
			string sEf02 = "";
			string sEf03 = "";

			string[,] aTypes = new string[3, 2] { { "Skill", "0" }, { "Stat", "1" }, { "Status", "3" } };
			string[,] aSkills = new string[10, 2] { { "Anal", "0" }, { "Magic", "1" }, { "BDSM", "2" }, { "Normal Sex", "3" }, { "Bestiality", "4" }, { "Group", "5" }, { "Lesbian", "6" }, { "Service", "7" }, { "Strip", "8" }, { "Combat", "9" } };
			string[,] aStats = new string[22, 2] { { "Charisma", "0" }, { "Happiness", "1" }, { "Libedo", "2" }, { "Constitution", "3" }, { "Intelligence", "4" }, { "Confidence", "5" }, { "Mana", "6" }, { "Agility", "7" }, { "Fame", "8" }, { "Level", "9" }, { "AskPrice", "10" }, { "House", "11" }, { "Experience", "12" }, { "Age", "13" }, { "Obedience", "14" }, { "Spirit", "15" }, { "Beauty", "16" }, { "Tiredness", "17" }, { "Health", "18" }, { "PC Fear", "19" }, { "PC Love", "20" }, { "PC Hate", "21" } };
			string[,] aStatus = new string[10, 2] { { "Poisoned", "1" }, { "Badly Poisoned", "2" }, { "Pregnant", "3" }, { "Pregnant By Player", "4" }, { "Slave", "5" }, { "Has daughter", "6" }, { "Has son", "7" }, { "Inseminated", "8" }, { "Controlled", "9" }, { "Catacombs", "10" } };

			textBox_ItemName.Text = ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][0].ToString();	   //name is already isolated so it's trivial to assign it to required textbox, it's in first cell ([0]) of datatable, only selected index is required, and since combobox is synchronised (or at least should be synchronized) with datatable it's easy to get name out
			string sData = ItemsCollection.Rows[listBox_ItemsList.SelectedIndex][1].ToString();				//this where it gets complicated, item data is in separate cell (this time [1]) from name so we need to parse it, first we'll put it in temp string, and after that disassemble it. Since this data isn't fixed length it would prove too complicated (my guess would be more to the line of impossible when considering DataTable nature) to put each line in separate column

			StringReader data = new StringReader(sData);							//item data is split by lines, so we pull it through StringReader to ease reading it line by line

			textBox_ItemDesc.Text = data.ReadLine();								//first line of data string is description so that's goes straight to description textbox
			string sIDesc = data.ReadLine();										//next line is item data line, this one needs to be parsed so it goes to string to wait disection

			char[] separator = { ' ' };											 //data in item data is space delimited, so to begin extraction from it we create an array of separator characters
			string[] values = sIDesc.Split(separator);							  //use Split function to put every value that's "between" spaces to array
																					//now that we have all 6 of required values safely tucked in their array spot we assign each value to it's place on item tab
			if (int.Parse(values[0]) < 1 || int.Parse(values[0]) > 11) comboBox_ItemType_01.SelectedIndex = 3;	  //Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Food/Consumable" if value is out of bounds
			else comboBox_ItemType_01.SelectedIndex = int.Parse(values[0]) - 1;	 //item type comboBox, it's "value - 1" because combobox index' go from 0 to 9, and item type index in game goes from 1 to 10, so to lower it to editor level... similarly when parsing items from tab values there's +1 for item type value

			trackBar_bad_02.Value = int.Parse(values[1]);						   //how bad is item for the girl trackbar
			lTrack1.Text = values[1];											   //label doesn't pick it up automaticaly when trackbar is set via program, so it also needs to be assign

			if (int.Parse(values[2]) < 0 || int.Parse(values[2]) > 2) comboBox_ItemDuration.SelectedIndex = 0;	  //Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Permanent" if value is out of bounds
			else comboBox_ItemDuration.SelectedIndex = int.Parse(values[2]);		//old unused "flag 3", now duration

			textBox_itemcost_04.Text = values[3];								   //item cost, just throw the string to textbox

			if (int.Parse(values[4]) < 0 || int.Parse(values[4]) > 6) comboBox_Rarity_05.SelectedIndex = 3;		 //Originaly there wasn't an "if" planned here, which works fine if input data is correct, but in the case it's out of bounds you get an error, so to prevent these from scaring users this if defaults the droplist to "Common" if value is out of bounds
			comboBox_Rarity_05.SelectedIndex = int.Parse(values[4]);				//rarity comboBox, unlike item type these go from 0 to 6, like index count for comboBox object

			if (values[5] == "0") checkBox_infinite_06.Checked = false;			 //item is infinite in marketplace or not, simple check and simple "switch"
			else checkBox_infinite_06.Checked = true;

			if (values.Length == 7)												 //GirlBuyChance value
			{
				checkBox_Item_GirlBuyChance.Checked = true;
				textBox_Item_GirlBuyChance.Text = values[6];
			}
			else
			{
				checkBox_Item_GirlBuyChance.Checked = false;
				textBox_Item_GirlBuyChance.Text = "";
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
					if (change == "1") change = "Add";
					else change = "Remove";
					iTTable.Rows.Add(trait, change);
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

					if (values[0] == "0")
					{
						j = 0;
						while (j < 10)
						{
							if (values[1] == aSkills[j, 1])
							{
								sEf02 = aSkills[j, 0];
								break;
							}
							else j = j + 1;
						}
					}
					else if (values[0] == "1")
					{
						j = 0;
						while (j < 22)
						{
							if (values[1] == aStats[j, 1])
							{
								sEf02 = aStats[j, 0];
								break;
							}
							else j = j + 1;
						}
					}
					else if (values[0] == "3")
					{
						j = 0;
						while (j < 9)
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
						if (values[2] == "1") sEf03 = "Add";
						else sEf03 = "Remove";
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

			try
			{
				Filesave.ShowDialog();
				if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
				if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");
				SaveItemsXML(Filesave.FileName);
				StatusLabel1.Text = "Successfully compiled " + listBox_ItemsList.Items.Count.ToString() + " items...";
			}
			catch
			{
			}

/*
			Filesave.FileName = "";
			Filesave.Filter = "Whore Master XML items file|*.itemsx|Whore Master items file|*.items|All files|*.*";

			try
			{
				Filesave.ShowDialog();
				if (File.Exists(Convert.ToString(Filesave.FileName) + ".bak") == true) File.Delete(Convert.ToString(Filesave.FileName) + ".bak");
				if (File.Exists(Convert.ToString(Filesave.FileName)) == true) File.Move(Convert.ToString(Filesave.FileName), Convert.ToString(Filesave.FileName) + ".bak");

				switch (Path.GetExtension(Filesave.FileName))
				{
					case ".items":
						SaveItems(Filesave.FileName);
						break;
					case ".itemsx":
						SaveItemsXML(Filesave.FileName);
						break;
				}
				StatusLabel1.Text = "Successfully compiled " + listBox_ItemsList.Items.Count.ToString() + " items...";
			}
			catch
			{
			}
*/			
		}

		//save in old format
		private void SaveItems(string path)
		{
			string output = "";
			for (int i = 0; i < listBox_ItemsList.Items.Count; i++)
			{
				output = output + ItemsCollection.Rows[i][0] + "\r\n" + ItemsCollection.Rows[i][1] + "\r\n";
			}
			output = output.TrimEnd('\r', '\n');

			StreamWriter Export = new StreamWriter(path);
			Export.Write(output);
			Export.Close();

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

				string[] sValues = sData.ReadLine().Split(' ');

				switch (sValues[0])
				{
					case "1":
						sValues[0] = "Ring";
						break;
					case "2":
						sValues[0] = "Dress";
						break;
					case "3":
						sValues[0] = "Shoes";
						break;
					case "4":
						sValues[0] = "Food";
						break;
					case "5":
						sValues[0] = "Necklace";
						break;
					case "6":
						sValues[0] = "Weapon";
						break;
					case "7":
						sValues[0] = "Makeup";
						break;
					case "8":
						sValues[0] = "Armor";
						break;
					case "9":
						sValues[0] = "Misc";
						break;
					case "10":
						sValues[0] = "Armband";
						break;
					case "11":
						sValues[0] = "Small Weapon";
						break;
				}
				switch (sValues[2])
				{
					case "0":
						sValues[2] = "None";
						break;
					case "1":
						sValues[2] = "AffectsAll";
						break;
					case "2":
						sValues[2] = "Temporary";
						break;
				}
				switch (sValues[4])
				{
					case "0":
						sValues[4] = "Common";
						break;
					case "1":
						sValues[4] = "Shop50";
						break;
					case "2":
						sValues[4] = "Shop25";
						break;
					case "3":
						sValues[4] = "Shop05";
						break;
					case "4":
						sValues[4] = "Catacomb15";
						break;
					case "5":
						sValues[4] = "ScriptOnly";
						break;
					case "6":
						sValues[4] = "ScriptOrReward";
						break;
				}
				switch (sValues[5])
				{
					case "0":
						sValues[5] = "false";
						break;
					case "1":
						sValues[5] = "true";
						break;
				}


				item.SetAttribute("Name", sName);
				item.SetAttribute("Desc", sDesc);
				item.SetAttribute("Type", sValues[0]);
				item.SetAttribute("Badness", sValues[1]);
				item.SetAttribute("Special", sValues[2]);
				item.SetAttribute("Cost", sValues[3]);
				item.SetAttribute("Rarity", sValues[4]);
				item.SetAttribute("Infinite", sValues[5]);

				if (sValues.Length == 7)									//check to see if item is old item that doesn't have girlbuychance
				{
					item.SetAttribute("GirlBuyChance", sValues[6]);
				}
				
				int num = Convert.ToInt32(sData.ReadLine());

				for (int y = 0; y < num; y++)
				{
					effect = xmldoc.CreateElement("Effect");

					string sEffectTest = sData.ReadLine();
					string[] sEffects; 

					if (IsNumeric(sEffectTest) == true && Convert.ToInt32(sEffectTest) == 4)
					{
						sEffects = new string[3];
						sEffects[0] = "Trait";
						sEffects[1] = sData.ReadLine();
						sEffects[2] = sData.ReadLine();
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
									case "0":
										sEffects[1] = "Anal";
										break;
									case "1":
										sEffects[1] = "Magic";
										break;
									case "2":
										sEffects[1] = "BDSM";
										break;
									case "3":
										sEffects[1] = "NormalSex";
										break;
									case "4":
										sEffects[1] = "Beastiality";
										break;
									case "5":
										sEffects[1] = "Group";
										break;
									case "6":
										sEffects[1] = "Lesbian";
										break;
									case "7":
										sEffects[1] = "Service";
										break;
									case "8":
										sEffects[1] = "Strip";
										break;
									case "9":
										sEffects[1] = "Combat";
										break;
								}
								break;
							case "1":
								sEffects[0] = "Stat";
								switch (sEffects[1])
								{
									case "0":
										sEffects[1] = "Charisma";
										break;
									case "1":
										sEffects[1] = "Happiness";
										break;
									case "2":
										sEffects[1] = "Libido";
										break;
									case "3":
										sEffects[1] = "Constitution";
										break;
									case "4":
										sEffects[1] = "Intelligence";
										break;
									case "5":
										sEffects[1] = "Confidence";
										break;
									case "6":
										sEffects[1] = "Mana";
										break;
									case "7":
										sEffects[1] = "Agility";
										break;
									case "8":
										sEffects[1] = "Fame";
										break;
									case "9":
										sEffects[1] = "Level";
										break;
									case "10":
										sEffects[1] = "AskPrice";
										break;
									case "11":
										sEffects[1] = "House";
										break;
									case "12":
										sEffects[1] = "Exp";
										break;
									case "13":
										sEffects[1] = "Age";
										break;
									case "14":
										sEffects[1] = "Obedience";
										break;
									case "15":
										sEffects[1] = "Spirit";
										break;
									case "16":
										sEffects[1] = "Beauty";
										break;
									case "17":
										sEffects[1] = "Tiredness";
										break;
									case "18":
										sEffects[1] = "Health";
										break;
									case "19":
										sEffects[1] = "PCFear";
										break;
									case "20":
										sEffects[1] = "PCLove";
										break;
									case "21":
										sEffects[1] = "PCHate";
										break;
								}
								break;
							case "2":
								sEffects[0] = "Nothing";
								break;
							case "3":
								sEffects[0] = "GirlStatus";
								switch (sEffects[1])
								{
									case "1":
										sEffects[1] = "Poisoned";
										break;
									case "2":
										sEffects[1] = "Badly Poisoned";
										break;
									case "3":
										sEffects[1] = "Pregnant";
										break;
									case "4":
										sEffects[1] = "Pregnant By Player";
										break;
									case "5":
										sEffects[1] = "Slave";
										break;
									case "6":
										sEffects[1] = "Has Daughter";
										break;
									case "7":
										sEffects[1] = "Has Son";
										break;
									case "8":
										sEffects[1] = "Inseminated";
										break;
									case "9":
										sEffects[1] = "Controlled";
										break;
									case "10":
										sEffects[1] = "Catacombs";
										break;
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

					item.AppendChild(effect);

				}
				items.AppendChild(item);
			}

			xmldoc.AppendChild(items);

			//xmldoc.Save(path);

			XmlWriterSettings settings = new XmlWriterSettings();
			settings.Indent = true;
			settings.NewLineOnAttributes = true;
			settings.IndentChars = "\t";
			XmlWriter xmlwrite = XmlWriter.Create(path, settings);

			//XmlTextWriter xmlwrite = new XmlTextWriter(path, Encoding.UTF8);
			//xmlwrite.Formatting = Formatting.Indented;
			//xmlwrite.QuoteChar = '\'';
			//xmlwrite.IndentChar = '\t';
			//xmlwrite.Indentation = 1;
			

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
			ListBox listBox = (ListBox)sender;								  //creates new, dummy, listbox object from original (casting sender object to ListBox using (ListBox)), not really necessary, we could call original listbox directly, but this way it's more universal, I can just paste it without needing to worry about changing listbox name in the code
			int index = listBox.IndexFromPoint(e.Location);					 //e.Location gives us current coordinates of mouse, and .IndexFromPoint parses these coordinates to get index of item mouse is currently over
			if (index > -1 && index < listBox.Items.Count)					  //if index is larger than -1 (i.e. there's and item it list, first item has index of 0), and it's smaller than number of items in list do...
			{
				string tip = listBox.Items[index].ToString();				   //this line and following if is not really necessary, it would work with only toolTip1.SetToolTip(... but, only having that line has one serious downside, tooltip flickering
				if (tip != lastTip)											 //every tiny movement of the mouse changes the coordinates, and consequently invokes this procedure which then reapplies that tooltip. This shows like constant flickering of tooltip
				{															   //also, it makes items rather hard to click, I guess CPU is bussy with refreshing the tooltip, I had to click every item few times to make them marked. What this does is simple, it puts current item in string and then checks if it's the same as last item it got, if yes then there's no need to refresh the tooltip, if not then load new tooltip, ergo, no annoying flicker
					toolTip1.SetToolTip(listBox, new StringReader(ItemsCollection.Rows[index][1].ToString()).ReadLine());	//this is the only difference from previous tooltip function. Description is stored in dataTable at [index][1], along with other data, so to get it out we first need to identify coordinates (that's what [index][1] is for), then construct StringReader from that long string, and lastly read first line from that stream, this is description. Luckily it can be fitted in one line :P
					lastTip = tip;											  //updates lastTip string
				}
			}
		}

		//*********************************
		//*****   General functions   *****
		//*********************************

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

				textBox_Config_InitialMoney.Text = xmldoc.SelectSingleNode("/config/Initial/@Gold").Value;
				textBox_Config_InitialGirlMeet.Text = xmldoc.SelectSingleNode("/config/Initial/@GirlMeet").Value;
				textBox_Config_InitialSlaveHousePerc.Text = xmldoc.SelectSingleNode("/config/Initial/@SlaveHousePerc").Value;
				if (xmldoc.SelectSingleNode("/config/Initial/@AutoUseItems").Value.TrimEnd('%') == "true") checkBox_Config_AutoUseItems.Checked = true;
				else checkBox_Config_AutoUseItems.Checked = false;

				textBox_Config_IncomeExtortion.Text = xmldoc.SelectSingleNode("/config/Income/@ExtortionIncome").Value;
				textBox_Config_IncomeBrothel.Text = xmldoc.SelectSingleNode("/config/Income/@GirlsWorkBrothel").Value;
				textBox_Config_IncomeStreet.Text = xmldoc.SelectSingleNode("/config/Income/@GirlsWorkStreet").Value;
				textBox_Config_IncomeMovie.Text = xmldoc.SelectSingleNode("/config/Income/@MovieIncome").Value;
				textBox_Config_IncomeStripper.Text = xmldoc.SelectSingleNode("/config/Income/@StripperIncome").Value;
				textBox_Config_IncomeBarmaid.Text = xmldoc.SelectSingleNode("/config/Income/@BarmaidIncome").Value;
				textBox_Config_IncomeSlaveSales.Text = xmldoc.SelectSingleNode("/config/Income/@SlaveSales").Value;
				textBox_Config_IncomeItemSales.Text = xmldoc.SelectSingleNode("/config/Income/@ItemSales").Value;

				textBox_Config_ExpenseTraining.Text = xmldoc.SelectSingleNode("/config/Expenses/@Training").Value;
				textBox_Config_ExpenseMovie.Text = xmldoc.SelectSingleNode("/config/Expenses/@MovieCost").Value;
				textBox_Config_ExpenseActress.Text = xmldoc.SelectSingleNode("/config/Expenses/@ActressWages").Value;
				textBox_Config_ExpenseGoon.Text = xmldoc.SelectSingleNode("/config/Expenses/@GoonWages").Value;
				textBox_Config_ExpenseMatron.Text = xmldoc.SelectSingleNode("/config/Expenses/@MatronWages").Value;
				textBox_Config_ExpenseGirls.Text = xmldoc.SelectSingleNode("/config/Expenses/@GirlSupport").Value;
				textBox_Config_ExpenseConsumables.Text = xmldoc.SelectSingleNode("/config/Expenses/@Consumables").Value;
				textBox_Config_ExpenseItems.Text = xmldoc.SelectSingleNode("/config/Expenses/@Items").Value;
				textBox_Config_ExpenseSlaves.Text = xmldoc.SelectSingleNode("/config/Expenses/@SlavesBought").Value;
				textBox_Config_ExpenseBrothelPrice.Text = xmldoc.SelectSingleNode("/config/Expenses/@BuyBrothel").Value;
				textBox_Config_ExpenseBrothel.Text = xmldoc.SelectSingleNode("/config/Expenses/@BrothelSupport").Value;
				textBox_Config_ExpenseBar.Text = xmldoc.SelectSingleNode("/config/Expenses/@BarSupport").Value;
				textBox_Config_ExpenseCasino.Text = xmldoc.SelectSingleNode("/config/Expenses/@CasinoSupport").Value;
				textBox_Config_ExpenseBribes.Text = xmldoc.SelectSingleNode("/config/Expenses/@Bribes").Value;
				textBox_Config_ExpenseFines.Text = xmldoc.SelectSingleNode("/config/Expenses/@Fines").Value;
				textBox_Config_ExpenseAdvertising.Text = xmldoc.SelectSingleNode("/config/Expenses/@Advertising").Value;

				textBox_Config_GamblingOdds.Text = xmldoc.SelectSingleNode("/config/Gambling/@Odds").Value.TrimEnd('%');
				textBox_Config_GamblingBase.Text = xmldoc.SelectSingleNode("/config/Gambling/@Base").Value;
				textBox_Config_GamblingSpread.Text = xmldoc.SelectSingleNode("/config/Gambling/@Spread").Value;
				textBox_Config_GamblingCustomer.Text = xmldoc.SelectSingleNode("/config/Gambling/@CustomerFactor").Value;
				textBox_Config_GamblingHouse.Text = xmldoc.SelectSingleNode("/config/Gambling/@HouseFactor").Value;

				textBox_Config_ProstitutionRapeBrothel.Text = xmldoc.SelectSingleNode("/config/Prostitution/@RapeBrothel").Value.TrimEnd('%');
				textBox_Config_ProstitutionRapeStreets.Text = xmldoc.SelectSingleNode("/config/Prostitution/@RapeStreet").Value.TrimEnd('%');

				textBox_Config_PregnancyPlayer.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@PlayerChance").Value.TrimEnd('%');
				textBox_Config_PregnancyCustomer.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@CustomerChance").Value.TrimEnd('%');
				textBox_Config_PregnancyMonster.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@MonsterChance").Value.TrimEnd('%');
				textBox_Config_PregnancyGoodSex.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@GoodSexFactor").Value;
				textBox_Config_PregnancyGirlChance.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@ChanceOfGirl").Value.TrimEnd('%');
				textBox_Config_PregnancyWeeksPregnant.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@WeeksPregnant").Value;
				textBox_Config_PregnancyWeeksTillGrown.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@WeeksTillGrown").Value;
				textBox_Config_PregnancyCoolDown.Text = xmldoc.SelectSingleNode("/config/Pregnancy/@CoolDown").Value;

				textBox_Config_TaxRate.Text = xmldoc.SelectSingleNode("/config/Tax/@Rate").Value.TrimEnd('%');
				textBox_Config_TaxMinimum.Text = xmldoc.SelectSingleNode("/config/Tax/@Minimum").Value.TrimEnd('%');
				textBox_Config_TaxLaundry.Text = xmldoc.SelectSingleNode("/config/Tax/@Laundry").Value.TrimEnd('%');

				textBox_config_MaxRecruitList.Text = xmldoc.SelectSingleNode("/config/Gangs/@MaxRecruitList").Value.TrimEnd('%');
				textBox_config_StartRandom.Text = xmldoc.SelectSingleNode("/config/Gangs/@StartRandom").Value.TrimEnd('%');
				textBox_config_StartBoosted.Text = xmldoc.SelectSingleNode("/config/Gangs/@StartBoosted").Value.TrimEnd('%');
				textBox_config_InitMemberMin.Text = xmldoc.SelectSingleNode("/config/Gangs/@InitMemberMin").Value.TrimEnd('%');
				textBox_config_InitMemberMax.Text = xmldoc.SelectSingleNode("/config/Gangs/@InitMemberMax").Value.TrimEnd('%');
				textBox_config_ChanceRemoveUnwanted.Text = xmldoc.SelectSingleNode("/config/Gangs/@ChanceRemoveUnwanted").Value.TrimEnd('%');
				textBox_config_AddNewWeeklyMin.Text = xmldoc.SelectSingleNode("/config/Gangs/@AddNewWeeklyMin").Value.TrimEnd('%');
				textBox_config_AddNewWeeklyMax.Text = xmldoc.SelectSingleNode("/config/Gangs/@AddNewWeeklyMax").Value.TrimEnd('%');

				textBox_Config_FontNormal.Text = xmldoc.SelectSingleNode("/config/Fonts/@Normal").Value.TrimEnd('%');
				textBox_config_FontFixed.Text = xmldoc.SelectSingleNode("/config/Fonts/@Fixed").Value.TrimEnd('%');
				if (xmldoc.SelectSingleNode("/config/Fonts/@Antialias").Value.TrimEnd('%') == "true") checkBox_Config_Antialias.Checked = true;
				else checkBox_Config_Antialias.Checked = false;

				if (xmldoc.SelectSingleNode("/config/Debug/@LogAll").Value.TrimEnd('%') == "true") checkBox_config_LogAll.Checked = true;
				else checkBox_config_LogAll.Checked = false;
				if (xmldoc.SelectSingleNode("/config/Debug/@LogItems").Value.TrimEnd('%') == "true") checkBox_config_LogItems.Checked = true;
				else checkBox_config_LogItems.Checked = false;
				if (xmldoc.SelectSingleNode("/config/Debug/@LogGirls").Value.TrimEnd('%') == "true") checkBox_config_LogGirls.Checked = true;
				else checkBox_config_LogGirls.Checked = false;
				if (xmldoc.SelectSingleNode("/config/Debug/@LogRGirls").Value.TrimEnd('%') == "true") checkBox_config_LogRGirls.Checked = true;
				else checkBox_config_LogRGirls.Checked = false;
				if (xmldoc.SelectSingleNode("/config/Debug/@LogFonts").Value.TrimEnd('%') == "true") checkBox_config_LogFonts.Checked = true;
				else checkBox_config_LogFonts.Checked = false;


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
			catch
			{

			}

		}

		//saves config.xml
		private void SaveConfigXML(string path)
		{
			XmlDocument xmldoc = new XmlDocument();

			XmlElement xeConfig = xmldoc.CreateElement("config");
			XmlElement xeInitial = xmldoc.CreateElement("Initial");
			XmlElement xeIncome = xmldoc.CreateElement("Income");
			XmlElement xeExpenses = xmldoc.CreateElement("Expenses");
			XmlElement xeGambling = xmldoc.CreateElement("Gambling");
			XmlElement xeTax = xmldoc.CreateElement("Tax");
			XmlElement xePregnancy = xmldoc.CreateElement("Pregnancy");
			XmlElement xeProstitution = xmldoc.CreateElement("Prostitution");
			XmlElement xeGangs = xmldoc.CreateElement("Gangs");
			XmlElement xeFonts = xmldoc.CreateElement("Fonts");
			XmlElement xeDebug = xmldoc.CreateElement("Debug");

			XmlComment xcInitial = xmldoc.CreateComment("\n\t\n\tThese are general settings.\n\tGold is how much gold you start the game with.\n\tGirlMeet is the %chance you'll meet a girl when walking around town.\n\tSlaveHousePerc is the default House Percentage for slave girls.\n\tAutoUseItems is whether or not the game will try to automatically use the player's\n\t	items intelligently on girls each week. This feature needs more testing.\n\t\n\t");
			XmlComment xcIncome = xmldoc.CreateComment("\n\t\n\tThese are the numbers that will multiply the money from\n\tvarious sources of income. So setting\n\t\"GirlsWorkBrothel\" to \"0.5\" will reduce the cash your girls\n\tgenerate in the brothel by half. You can also use numbers\n\t>1 to increase income if you are so inclined.\n\t\n\t");
			XmlComment xcExpenses = xmldoc.CreateComment("\n\t\n\tThese are the mulipliers for your expenses.\n\tTraining doesn't currently have a cost,\n\tSo I'm setting it to 1 gold per girl per week\n\tand defaulting the multiplier to 0 (so no change\n\tby default). Set it higher and training begins to\n\tcost beyond the simple loss of income.\n\n\tActressWages are like training costs: a per-girl\n\texpense nominally 1 gold per girl, but with a default\n\tfactor of 0, so no change to the current scheme unless\n\tyou alter that. MakingMovies is the setup cost for\n\ta movie: I'm going to make this 1000 gold per movie,\n\tbut again, with a zero factor by default.\n\n\tOtherwise, same as above, except you probably\n\twant numbers > 1 to make things more expensive here.\n\n\tOne more exception: Tax rate is the % of your earnings\n\ttaken as tax.\n\t\n\t");
			XmlComment xcGambling = xmldoc.CreateComment("\n\t\n\tGambling:\n\n\tThe starting %chance for the tables is given by \"Odds\"\n\n\tWins and losses on the tables are\n\tcalculated as  the \"Base\" value + a random number\n\tbetween 1 and the value of \"Spread\". If the house\n\twins, the amount is mutiplied by the HouseFactor\n\tand if the customer wins, by the customer factor\n\n\tSo: if Base = 50 and spread = 100 then the basic amount\n\twon or lost per customer would be 50+d100.\n\n\tAs it stands, the default odds are near 50%\n\twhile the payout is 2:1 in favour of the house.\n\tSo by default, the tables are rigged!\n\t\n\t");
			XmlComment xcTax = xmldoc.CreateComment("\n\t\n\tTax:\n\n\tRate is the rate at which your income is taxed\n\tMin is the minumum adjusted rate after influence is\n\tused to lower the tax rate.\n\tLaundry is the Maximum % of your income that can be\n\tLaundered and so escape taxation.\n\t\n\t");
			XmlComment xcPregnancy = xmldoc.CreateComment("\n\t\n\tPlayer-, Customer- and MonsterChance\n\tgive the odds on her getting knocked up\n\tby the PC, a customer and a monster, respectvely\n\n\tGoodSexFactor is the multiplier for the pregnancy chance\n\tif both parties were happy afterward.\n\n\tChanceOfGirl is the %chance of any baby being female.\n\t\n\t");
			XmlComment xcGangs = xmldoc.CreateComment("\n\t\n\tGangs:\n\n\tMaxRecruitList limits the maximum number of recruitable\n\tgangs listed for you to hire.\n\tWARNING: BE CAREFUL here; the number of\n\trecruitable gangs plus the number of potential hired\n\tgangs must not exceed the number of names stored in\n\tHiredGangNames.txt. For example, with 20 names, you\n\tcould have a max of 12 recruitables since you have to\n\taccount for the possible 8 hired gangs.\n\n\t\n\tStartRandom is how many random recruitable gangs are\n\tcreated for you at the start of a new game. StartBoosted\n\tis how many stat-boosted starting gangs are also added.\n\n\t\n\tInitMemberMin and InitMemberMax indicate the number\n\tof initial gang members which are in each recruitable\n\tgang; a random number between Min and Max is picked.\n\n\t\n\tChanceRemoveUnwanted is the %chance each week that\n\teach unhired gang in the recruitable list is removed.\n\n\t\n\tAddNewWeeklyMin and AddNewWeeklyMax indicate\n\thow many new random gangs are added to the recruitable\n\tgangs list each week; a random number between Min and\n\tMax is picked.\n\t\n\t");
			XmlComment xcProstitution = xmldoc.CreateComment("\n\t\n\tthese are the base chances of rape in\n\tbrothel and streetwalking\n\t\n\t");
			XmlComment xcFonts = xmldoc.CreateComment("\n\t\n\tNormal is the font that the game uses for text\n\tit replaces the Font.txt file\n\n\t\n\tFixed is for a monospaced font for tabular info\n\tbut nothing currently uses that.\n\n\t\n\tAntialias determines whether font antialiasing (smoothing) is used\n\n\t\n\tIt's worth leaving these in, since once the XML screen format\n\tis stable, it will be possible to set custom fonts for different\n\ttext elements, just like designing a web page.\n\n\t\n\t(Except that you'll have to distribute the font with the game or mod\n\trather than relying on the viewer to have it pre-installed.)");

			xeConfig.AppendChild(xcInitial);
			xeInitial.SetAttribute("Gold", textBox_Config_InitialMoney.Text);
			xeInitial.SetAttribute("GirlMeet", textBox_Config_InitialGirlMeet.Text);
			xeInitial.SetAttribute("SlaveHousePerc", textBox_Config_InitialSlaveHousePerc.Text);
			if (checkBox_Config_AutoUseItems.Checked == true) xeInitial.SetAttribute("AutoUseItems", "true");
			else xeInitial.SetAttribute("AutoUseItems", "false");
			xeConfig.AppendChild(xeInitial);

			xeConfig.AppendChild(xcIncome);
			xeIncome.SetAttribute("ExtortionIncome", textBox_Config_IncomeExtortion.Text);
			xeIncome.SetAttribute("GirlsWorkBrothel", textBox_Config_IncomeBrothel.Text);
			xeIncome.SetAttribute("GirlsWorkStreet", textBox_Config_IncomeStreet.Text);
			xeIncome.SetAttribute("MovieIncome", textBox_Config_IncomeMovie.Text);
			xeIncome.SetAttribute("StripperIncome", textBox_Config_IncomeStripper.Text);
			xeIncome.SetAttribute("BarmaidIncome", textBox_Config_IncomeBarmaid.Text);
			xeIncome.SetAttribute("SlaveSales", textBox_Config_IncomeSlaveSales.Text);
			xeIncome.SetAttribute("ItemSales", textBox_Config_IncomeItemSales.Text);
			xeConfig.AppendChild(xeIncome);

			xeConfig.AppendChild(xcExpenses);
			xeExpenses.SetAttribute("Training", textBox_Config_ExpenseTraining.Text);
			xeExpenses.SetAttribute("MovieCost", textBox_Config_ExpenseMovie.Text);
			xeExpenses.SetAttribute("ActressWages", textBox_Config_ExpenseActress.Text);
			xeExpenses.SetAttribute("GoonWages", textBox_Config_ExpenseGoon.Text);
			xeExpenses.SetAttribute("MatronWages", textBox_Config_ExpenseMatron.Text);
			xeExpenses.SetAttribute("GirlSupport", textBox_Config_ExpenseGirls.Text);
			xeExpenses.SetAttribute("Consumables", textBox_Config_ExpenseConsumables.Text);
			xeExpenses.SetAttribute("Items", textBox_Config_ExpenseItems.Text);
			xeExpenses.SetAttribute("SlavesBought", textBox_Config_ExpenseSlaves.Text);
			xeExpenses.SetAttribute("BuyBrothel", textBox_Config_ExpenseBrothelPrice.Text);
			xeExpenses.SetAttribute("BrothelSupport", textBox_Config_ExpenseBrothel.Text);
			xeExpenses.SetAttribute("BarSupport", textBox_Config_ExpenseBar.Text);
			xeExpenses.SetAttribute("CasinoSupport", textBox_Config_ExpenseCasino.Text);
			xeExpenses.SetAttribute("Bribes", textBox_Config_ExpenseBribes.Text);
			xeExpenses.SetAttribute("Fines", textBox_Config_ExpenseFines.Text);
			xeExpenses.SetAttribute("Advertising", textBox_Config_ExpenseAdvertising.Text);
			xeConfig.AppendChild(xeExpenses);

			xeConfig.AppendChild(xcGambling);
			xeGambling.SetAttribute("Odds", textBox_Config_GamblingOdds.Text + "%");
			xeGambling.SetAttribute("Base", textBox_Config_GamblingBase.Text);
			xeGambling.SetAttribute("Spread", textBox_Config_GamblingSpread.Text);
			xeGambling.SetAttribute("CustomerFactor", textBox_Config_GamblingCustomer.Text);
			xeGambling.SetAttribute("HouseFactor", textBox_Config_GamblingHouse.Text);
			xeConfig.AppendChild(xeGambling);

			xeConfig.AppendChild(xcTax);
			xeTax.SetAttribute("Rate", textBox_Config_TaxRate.Text + "%");
			xeTax.SetAttribute("Minimum", textBox_Config_TaxMinimum.Text + "%");
			xeTax.SetAttribute("Laundry", textBox_Config_TaxLaundry.Text + "%");
			xeConfig.AppendChild(xeTax);

			xeConfig.AppendChild(xcPregnancy);
			xePregnancy.SetAttribute("PlayerChance", textBox_Config_PregnancyPlayer.Text + "%");
			xePregnancy.SetAttribute("CustomerChance", textBox_Config_PregnancyCustomer.Text + "%");
			xePregnancy.SetAttribute("MonsterChance", textBox_Config_PregnancyMonster.Text + "%");
			xePregnancy.SetAttribute("GoodSexFactor", textBox_Config_PregnancyGoodSex.Text);
			xePregnancy.SetAttribute("ChanceOfGirl", textBox_Config_PregnancyGirlChance.Text + "%");
			xePregnancy.SetAttribute("WeeksPregnant", textBox_Config_PregnancyWeeksPregnant.Text);
			xePregnancy.SetAttribute("WeeksTillGrown", textBox_Config_PregnancyWeeksTillGrown.Text);
			xePregnancy.SetAttribute("CoolDown", textBox_Config_PregnancyCoolDown.Text);
			xeConfig.AppendChild(xePregnancy);

			xeConfig.AppendChild(xcProstitution);
			xeProstitution.SetAttribute("RapeBrothel", textBox_Config_ProstitutionRapeBrothel.Text + "%");
			xeProstitution.SetAttribute("RapeStreet", textBox_Config_ProstitutionRapeStreets.Text + "%");
			xeConfig.AppendChild(xeProstitution);

			xeConfig.AppendChild(xcGangs);
			xeGangs.SetAttribute("MaxRecruitList", textBox_config_MaxRecruitList.Text);
			xeGangs.SetAttribute("StartRandom", textBox_config_StartRandom.Text);
			xeGangs.SetAttribute("StartBoosted", textBox_config_StartBoosted.Text);
			xeGangs.SetAttribute("InitMemberMin", textBox_config_InitMemberMin.Text);
			xeGangs.SetAttribute("InitMemberMax", textBox_config_InitMemberMax.Text);
			xeGangs.SetAttribute("ChanceRemoveUnwanted", textBox_config_ChanceRemoveUnwanted.Text + "%");
			xeGangs.SetAttribute("AddNewWeeklyMin", textBox_config_AddNewWeeklyMin.Text);
			xeGangs.SetAttribute("AddNewWeeklyMax", textBox_config_AddNewWeeklyMax.Text);
			xeConfig.AppendChild(xeGangs);

			xeConfig.AppendChild(xcFonts);
			xeFonts.SetAttribute("Normal", textBox_Config_FontNormal.Text);
			xeFonts.SetAttribute("Fixed", textBox_config_FontFixed.Text);
			if (checkBox_Config_Antialias.Checked == true) xeFonts.SetAttribute("Antialias", "true");
			else xeFonts.SetAttribute("Antialias", "false");
			xeConfig.AppendChild(xeFonts);

			if (checkBox_config_LogAll.Checked == true) xeDebug.SetAttribute("LogAll", "true");
			else xeDebug.SetAttribute("LogAll", "false");
			if (checkBox_config_LogItems.Checked == true) xeDebug.SetAttribute("LogItems", "true");
			else xeDebug.SetAttribute("LogItems", "false");
			if (checkBox_config_LogGirls.Checked == true) xeDebug.SetAttribute("LogGirls", "true");
			else xeDebug.SetAttribute("LogGirls", "false");
			if (checkBox_config_LogRGirls.Checked == true) xeDebug.SetAttribute("LogRGirls", "true");
			else xeDebug.SetAttribute("LogRGirls", "false");
			if (checkBox_config_LogFonts.Checked == true) xeDebug.SetAttribute("LogFonts", "true");
			else xeDebug.SetAttribute("LogFonts", "false");
			xeConfig.AppendChild(xeDebug);

			xmldoc.AppendChild(xeConfig);

			XmlWriterSettings settings = new XmlWriterSettings();
			settings.Indent = true;
			settings.NewLineOnAttributes = true;
			settings.IndentChars = "\t";
			XmlWriter xmlwrite = XmlWriter.Create(path, settings);


			xmldoc.Save(xmlwrite);
			xmlwrite.Close();
		}

		//resets to default
		private void ConfigReset()
		{
			textBox_Config_InitialMoney.Text = "4000";

			textBox_Config_IncomeExtortion.Text = "1.0";
			textBox_Config_IncomeBrothel.Text = "1.0";
			textBox_Config_IncomeStreet.Text = "1.0";
			textBox_Config_IncomeMovie.Text = "1.0";
			textBox_Config_IncomeStripper.Text = "1.0";
			textBox_Config_IncomeBarmaid.Text = "1.0";
			textBox_Config_IncomeSlaveSales.Text = "1.0";
			textBox_Config_IncomeItemSales.Text = "1.0";

			textBox_Config_ExpenseTraining.Text = "0.0";
			textBox_Config_ExpenseMovie.Text = "0.0";
			textBox_Config_ExpenseActress.Text = "0.0";
			textBox_Config_ExpenseGoon.Text = "1.0";
			textBox_Config_ExpenseMatron.Text = "1.0";
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

			textBox_Config_ProstitutionRapeBrothel.Text = "2";
			textBox_Config_ProstitutionRapeStreets.Text = "5";

			textBox_Config_PregnancyPlayer.Text = "8";
			textBox_Config_PregnancyCustomer.Text = "8";
			textBox_Config_PregnancyMonster.Text = "8";
			textBox_Config_PregnancyGoodSex.Text = "2.0";
			textBox_Config_PregnancyGirlChance.Text = "50";
			textBox_Config_PregnancyWeeksPregnant.Text = "38";
			textBox_Config_PregnancyWeeksTillGrown.Text = "60";
			textBox_Config_PregnancyCoolDown.Text = "4";

			textBox_Config_TaxRate.Text = "6";
			textBox_Config_TaxMinimum.Text = "1";
			textBox_Config_TaxLaundry.Text = "25";
		}

		//save config button
		private void button_Config_Save_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveConfig = new SaveFileDialog();
			saveConfig.Filter = "Whore Master XML config file|*.xml|All files|*.*";
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
			catch
			{

			}
			
		}

		//reset config button
		private void button_Config_Reset_Click(object sender, EventArgs e)
		{
			ConfigReset();
		}

		//*********************************
		//*****   General functions   *****
		//*********************************

		//complicated way to sort DataTable :P, currently unused, but if need be it can stay here
		private static void SortDataTable_old(DataTable dt, string sort)
		{
			DataTable newDT = dt.Clone();
			int rowCount = dt.Rows.Count;

			DataRow[] foundRows = dt.Select(null, sort); // Sort with Column name
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
			dt = v.ToTable();									   //apply this sorted view to our original DataTable

			lb.Items.Clear();									   //empty listbBox from entries it has

			for (int x = 0; x < dt.Rows.Count; x++)				 //go through all records in DataTable and add names to listBox so our index sync works again
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
						tabControl1.SelectedTab = tabPage1;
						break;
					case ".girlsx":
						LoadGirlsXML(file);
						tabControl1.SelectedTab = tabPage1;
						break;
					case ".rgirls":
						LoadRGirls(file);
						tabControl1.SelectedTab = tabPage5;
						break;
					case ".rgirlsx":
						LoadRGirlsXML(file);
						tabControl1.SelectedTab = tabPage5;
						break;
					case ".items":
						LoadItems(file);
						tabControl1.SelectedTab = tabPage6;
						break;
					case ".itemsx":
						LoadItemsXML(file);
						tabControl1.SelectedTab = tabPage6;
						break;
				}
				
			}
		}

		private void textBox_Config_IncomeExtortion_TextChanged(object sender, EventArgs e)
		{

		}

		private void groupBox24_Enter(object sender, EventArgs e)
		{

		}

		private void label142_Click(object sender, EventArgs e)
		{

		}

		private void label28_Click(object sender, EventArgs e)
		{

		}
	}
}