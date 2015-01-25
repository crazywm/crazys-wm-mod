using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Text.RegularExpressions;

//      To make form1.cs file more manageable I've moved textbox events to this file

namespace WM_Girls_Generator
{
    public partial class Form1
    {

        private void TBox_G_RealName_TextChanged(object sender, EventArgs e)
        {
            bool imagesfound = false;
            string sCharactersPath = "Resources\\Characters\\";
            string b = sCharactersPath;
            string c = TBox_G_RealName.Text;
            string d = b + c;
            string f = "";
            if (Directory.Exists(d))
            {
                f = d;
                if (Directory.GetFiles(f, "*.*g").Count() + Directory.GetFiles(f, "*.ani").Count() > 0)
                    imagesfound = true;
            }
            else if (Directory.Exists("..\\" + d))
            {
                f = "..\\" + d;
                if (Directory.GetFiles(f, "*.*g").Count() + Directory.GetFiles(f, "*.ani").Count() > 0)
                    imagesfound = true;
            }
            else if (Directory.Exists("..\\..\\" + d))
            {
                f = "..\\..\\" + d;
                if (Directory.GetFiles(f, "*.*g").Count() + Directory.GetFiles(f, "*.ani").Count() > 0)
                    imagesfound = true;
            }
            else if (Directory.Exists("..\\..\\..\\" + d))
            {
                f = "..\\..\\..\\" + d;
                if (Directory.GetFiles(f, "*.*g").Count() + Directory.GetFiles(f, "*.ani").Count() > 0)
                    imagesfound = true;
            }
            else if (Directory.Exists("..\\..\\..\\..\\" + d))
            {
                f = "..\\..\\..\\..\\" + d;
                if (Directory.GetFiles(f, "*.*g").Count() + Directory.GetFiles(f, "*.ani").Count() > 0)
                    imagesfound = true;
            }

            if (imagesfound)
            {
                int count = Directory.GetFiles(f, "*.jpg").Count() +    Directory.GetFiles(f, "*.jpeg").Count() +
                            Directory.GetFiles(f, "*.png").Count() +    Directory.GetFiles(f, "*.ani").Count();

                label_G_Images_Found.Text = "Images Found: " + count.ToString();
            }
            else
                label_G_Images_Found.Text = "Images Found: 0";
        }







        /*
         * limits input to integers and limits it to 0 to 100 values, 
         * rough way, but it works, it would take quite some time to change it to different way 
         * (like the ones on bottom that have two events for every textbox, 
         *      both keypress and textchanged, maybe I'll change it eventually)
         *      
         * `J` removed "LimitTextBoxRMax" and "LimitTextBoxRMin" variants that did the same thing
        */
        private void LimitTextBox(TextBox tbox) 
        {
            if (tbox.Text.Length > 0)
            {
                try
                {
                    int test = int.Parse(tbox.Text);
                }
                catch
                {
                    tbox.Text = tbox.Text.Substring(0, tbox.Text.Length - 1);
                    tbox.SelectionStart = tbox.Text.Length;
                    tbox.ScrollToCaret();
                }
                if (tbox.Text.Length < 1) { tbox.Text = "0"; }   // `J` added this to prevent crash.
                else if (int.Parse(tbox.Text) > 100) tbox.Text = "100";
            }
        }

        private void ValidateTextBox(object sender, EventArgs e)
        {
            TextBox textBox = (TextBox)sender;
            int test=0;
            if (textBox.Text.Length > 0)
            {
                try { test = int.Parse(textBox.Text); } catch { test = 0; }
            }
            if (test < 0) test = 0; if (test > 100) test = 100;
            textBox.Text = test.ToString();
        }
        private void LimitTextBox(object sender, KeyPressEventArgs e)
        {
            e.Handled = true;  // everything is handled by this process so the program does not need to add the key itself
            TextBox textBox = (TextBox)sender;

            if (e.KeyChar == (char)Keys.Back) textBox.Text = "";

            else if (char.IsDigit(e.KeyChar))
            {
                int testnum = 0; string teststr = "";
                // check the existing ...
                for (int i = 0; i < textBox.Text.Length; i++)
                {
                    teststr += textBox.Text.Substring(i, 1);
                }
                // process the new key
                teststr += e.KeyChar;

                testnum = (teststr.Length > 0) ? int.Parse(teststr) : 0;
                if (testnum > 100) testnum = 100;
                textBox.Text = testnum.ToString();
            }
        }

        private void ValidateTextBoxNoMax(object sender, EventArgs e)
        {
            TextBox textBox = (TextBox)sender;
            int test = 0;
            if (textBox.Text.Length > 0)
            {
                try { test = int.Parse(textBox.Text); } catch { test = 0; }
            }
            if (test < 0) test = 0;
            textBox.Text = test.ToString();
        }
        private void LimitTextBoxNoMax(object sender, KeyPressEventArgs e)
        {
            e.Handled = true;  // everything is handled by this process so the program does not need to add the key itself
            TextBox textBox = (TextBox)sender;

            if (e.KeyChar == (char)Keys.Back) textBox.Text = "";

            else if (char.IsDigit(e.KeyChar) && textBox.Text.Length<6)
            {
                int testnum = 0; string teststr = "";
                // check the existing ...
                for (int i = 0; i < textBox.Text.Length; i++)
                {
                    teststr += textBox.Text.Substring(i, 1);
                }
                // process the new key
                teststr += e.KeyChar;

                testnum = (teststr.Length > 0) ? int.Parse(teststr) : 0;
                if (testnum < 0) testnum = 0;
                textBox.Text = testnum.ToString();
            }
        }

        private void ValidateTextBoxNegative(object sender, EventArgs e)
        {
            TextBox textBox = (TextBox)sender;
            int test = 0;
            if (textBox.Text.Length > 0)
            {
                try { test = int.Parse(textBox.Text); } catch { test = 0; }
            }
            if (test < -100) test = -100; if (test > 100) test = 100;
            textBox.Text = test.ToString();
        }
        private void LimitTextBoxNegative(object sender, KeyPressEventArgs e)
        {
            e.Handled = true;  // everything is handled by this process so the program does not need to add the key itself
            TextBox textBox = (TextBox)sender;

            if (e.KeyChar == (char)Keys.Back) textBox.Text = "";

            else if (e.KeyChar == '-' || char.IsDigit(e.KeyChar))
            {
                int testnum = 0; string teststr = ""; bool negative = false;
                // check the existing ...
                for (int i = 0; i < textBox.Text.Length; i++)
                {
                    if (textBox.Text.Substring(i, 1) == "-") negative = true;
                    else teststr += textBox.Text.Substring(i, 1);
                }
                // process the new key
                if (e.KeyChar == '-') negative = (negative) ? false : true;
                if (char.IsDigit(e.KeyChar)) teststr += e.KeyChar;

                testnum = (teststr.Length > 0) ? int.Parse(teststr) : 0;
                if (testnum > 100) testnum = 100;
                if (negative && testnum != 0) testnum *= -1;

                if (testnum == 0) textBox.Text = (negative) ? "-" : "0";
                else textBox.Text = testnum.ToString();
            }
        }


        private void ValidateTextBoxDouble(object sender, EventArgs e)
        {
            TextBox textBox = (TextBox)sender;

            string teststr = ""; bool dotfound = false;
            for (int i = 0; i < textBox.Text.Length; i++)
            {
                if (!dotfound && textBox.Text.Substring(i, 1) == ".")
                {
                    dotfound = true;
                    teststr += textBox.Text.Substring(i, 1);
                }
                else if (dotfound && textBox.Text.Substring(i, 1) == ".") { }
                else teststr += textBox.Text.Substring(i, 1);
            }
            double test = 0.0;
            if (textBox.Text.Length > 0)
            {
                try { test = double.Parse(textBox.Text); } catch { test = 0.0; }
            }
            if (test < 0.0 || test > 100.0)
            {

                if (test < 0.0) test = 0.0;
                if (test > 100.0) test = 100.0;
                textBox.Text = test.ToString();
            }
        }
        private void LimitTextBoxDouble(object sender, KeyPressEventArgs e)
        {
            e.Handled = true;  // everything is handled by this process so the program does not need to add the key itself
            TextBox textBox = (TextBox)sender;
            if (e.KeyChar == (char)Keys.Back) textBox.Text = "";
            else if ((e.KeyChar == '.' || char.IsDigit(e.KeyChar)) && textBox.Text.Length < 5)
            {
                double testnum = 0.0; string teststr = ""; bool dotfound = false;
                if (textBox.Text.Length < 1)
                {
                    textBox.Text = "";
                    if (e.KeyChar == '.') textBox.Text += "0.";
                    else textBox.Text += e.KeyChar;
                }
                else
                {
                    // check the existing ...
                    for (int i = 0; i < textBox.Text.Length; i++)
                    {
                        if (!dotfound && textBox.Text.Substring(i, 1) == ".")
                        {
                            dotfound = true;
                            teststr += textBox.Text.Substring(i, 1);
                        }
                        else if (dotfound && textBox.Text.Substring(i, 1) == ".") { }
                        else teststr += textBox.Text.Substring(i, 1);
                    }
                    // process the new key

                    if (char.IsDigit(e.KeyChar) || (!dotfound && e.KeyChar == '.')) teststr += e.KeyChar;
                    if (textBox.Text.Length > 0)
                    {
                        try { testnum = double.Parse(textBox.Text); } catch { testnum = 0.0; }
                    }
                    if (testnum < 0.0 || testnum > 100.0)
                    {

                        if (testnum < 0.0) testnum = 0.0;
                        if (testnum > 100.0) testnum = 100.0;
                        textBox.Text = testnum.ToString();
                    }
                    else textBox.Text = teststr;
                }
            }
        }


        
        public static bool IsNumeric(string strToCheck) //function to check if string is numeric or not
        {
            return Regex.IsMatch(strToCheck, "^\\d+(\\.\\d+)?$");
        }


        //checks keypresses to allow only numbers
        private void GoldTBox1_KeyPress(object sender, KeyPressEventArgs e)                 
        {
            if ((e.KeyChar != (char)Keys.Back && !char.IsDigit(e.KeyChar)))
            {
                e.Handled = true;
            }
        }
        //if everything in this textbox is deleted it puts 0 in it
        private void GoldTBox1_TextChanged(object sender, EventArgs e)                      
        {
            if (GoldTBox1.Text.Length == 0) GoldTBox1.Text = "0";
        }


        private void affects_textBox_value_TextChanged(object sender, EventArgs e)          //if everything in this textbox is deleted it puts 0 in it
        {
            if (affects_textBox_value.Text.Length == 0) affects_textBox_value.Text = "0";
        }

        private void textBox_itemcost_04_KeyPress(object sender, KeyPressEventArgs e)       //checks keypresses to allow only numbers
        {
            if ((e.KeyChar != (char)Keys.Back && !char.IsDigit(e.KeyChar)))
            {
                e.Handled = true;
            }
        }

        private void textBox_itemcost_04_TextChanged(object sender, EventArgs e)            //if everything in this textbox is deleted it puts 0 in it
        {
            if (textBox_itemcost_04.Text.Length == 0) textBox_itemcost_04.Text = "0";
        }

        private void textBox_RGTraitValue_KeyPress(object sender, KeyPressEventArgs e)      //checks keypresses to allow only numbers
        {
            if ((e.KeyChar != (char)Keys.Back && !char.IsDigit(e.KeyChar)))
            {
                e.Handled = true;
            }
        }

        private void textBox_RGTraitValue_TextChanged(object sender, EventArgs e)           //if everything in this textbox is deleted it puts 0 in it
        {
            if (textBox_RGTraitValue.Text.Length == 0) textBox_RGTraitValue.Text = "0";
            if (Convert.ToInt32(textBox_RGTraitValue.Text) > 65535) textBox_RGTraitValue.Text = "65535";
        }

        private void textBox_Item_GirlBuyChance_TextChanged(object sender, EventArgs e)
        {
            //if (textBox_Item_GirlBuyChance.Text.Length == 0) textBox_Item_GirlBuyChance.Text = "0";
            //if (Convert.ToInt32(textBox_Item_GirlBuyChance.Text) > 65535) textBox_Item_GirlBuyChance.Text = "100";
            LimitTextBox(textBox_Item_GirlBuyChance);
        }

        private void textBox_Item_GirlBuyChance_KeyPress(object sender, KeyPressEventArgs e)
        {
            if ((e.KeyChar != (char)Keys.Back && !char.IsDigit(e.KeyChar)))
            {
                e.Handled = true;
            }
        }
    }
}