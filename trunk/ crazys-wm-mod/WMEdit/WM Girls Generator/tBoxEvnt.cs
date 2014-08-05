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

        public static bool IsNumeric(string strToCheck)                         //function to check if string is numeric or not
        {
            return Regex.IsMatch(strToCheck, "^\\d+(\\.\\d+)?$");
        }
        private void StatsTBox_01_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_01); }
        private void StatsTBox_02_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_02); }
        private void StatsTBox_03_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_03); }
        private void StatsTBox_04_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_04); }
        private void StatsTBox_05_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_05); }
        private void StatsTBox_06_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_06); }
        private void StatsTBox_07_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_07); }
        private void StatsTBox_08_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_08); }
        private void StatsTBox_09_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_09); }
        private void StatsTBox_10_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_G_Level); }
        private void StatsTBox_11_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_11); }
        private void StatsTBox_12_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_12); }
        private void StatsTBox_13_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_G_Exp); }
        private void StatsTBox_14_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_14); }
        private void StatsTBox_15_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_15); }
        private void StatsTBox_16_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_16); }
        private void StatsTBox_17_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_17); }
        private void StatsTBox_18_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_18); }
        private void StatsTBox_19_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_19); }
        private void StatsTBox_20_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_20); }
        private void StatsTBox_21_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_21); }
        private void StatsTBox_22_TextChanged(object sender, EventArgs e) { LimitTextBox(StatsTBox_22); }

        private void SkillTBox_01_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_01); }
        private void SkillTBox_02_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_02); }
        private void SkillTBox_03_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_03); }
        private void SkillTBox_04_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_04); }
        private void SkillTBox_05_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_05); }
        private void SkillTBox_06_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_06); }
        private void SkillTBox_07_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_07); }
        private void SkillTBox_08_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_08); }
        private void SkillTBox_09_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_09); }
        private void SkillTBox_10_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_10); }
        private void SkillTBox_11_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_11); }
        private void SkillTBox_12_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_12); }
        private void SkillTBox_13_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_13); }
        private void SkillTBox_14_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_14); }
        private void SkillTBox_15_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_15); }
        private void SkillTBox_16_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_16); }
        private void SkillTBox_17_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_17); }
        private void SkillTBox_18_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_18); }
        private void SkillTBox_19_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_19); }
        private void SkillTBox_20_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillTBox_20); }

        private void StatRGMinTBox1_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox1); }
        private void StatRGMinTBox2_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox2); }
        private void StatRGMinTBox3_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox3); }
        private void StatRGMinTBox4_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox4); }
        private void StatRGMinTBox5_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox5); }
        private void StatRGMinTBox6_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox6); }
        private void StatRGMinTBox7_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox7); }
        private void StatRGMinTBox8_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox8); }
        private void StatRGMinTBox9_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox9); }
        private void StatRGMinTBox10_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox10); }
        private void StatRGMinTBox11_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox11); }
        private void StatRGMinTBox12_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox12); }
        private void StatRGMinTBox13_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox13); }
        private void StatRGMinTBox14_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox14); }
        private void StatRGMinTBox15_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox15); }
        private void StatRGMinTBox16_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox16); }
        private void StatRGMinTBox17_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox17); }
        private void StatRGMinTBox18_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox18); }
        private void StatRGMinTBox19_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox19); }
        private void StatRGMinTBox20_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox20); }
        private void StatRGMinTBox21_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox21); }
        private void StatRGMinTBox22_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMinTBox22); }

        private void StatRGMaxTBox1_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox1); }
        private void StatRGMaxTBox2_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox2); }
        private void StatRGMaxTBox3_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox3); }
        private void StatRGMaxTBox4_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox4); }
        private void StatRGMaxTBox5_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox5); }
        private void StatRGMaxTBox6_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox6); }
        private void StatRGMaxTBox7_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox7); }
        private void StatRGMaxTBox8_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox8); }
        private void StatRGMaxTBox9_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox9); }
        private void StatRGMaxTBox10_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox10); }
        private void StatRGMaxTBox11_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox11); }
        private void StatRGMaxTBox12_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox12); }
        private void StatRGMaxTBox13_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox13); }
        private void StatRGMaxTBox14_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox14); }
        private void StatRGMaxTBox15_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox15); }
        private void StatRGMaxTBox16_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox16); }
        private void StatRGMaxTBox17_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox17); }
        private void StatRGMaxTBox18_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox18); }
        private void StatRGMaxTBox19_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox19); }
        private void StatRGMaxTBox20_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox20); }
        private void StatRGMaxTBox21_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox21); }
        private void StatRGMaxTBox22_TextChanged(object sender, EventArgs e) { LimitTextBox(StatRGMaxTBox22); }

        private void SkillRGMinTBox1_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox1); }
        private void SkillRGMinTBox2_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox2); }
        private void SkillRGMinTBox3_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox3); }
        private void SkillRGMinTBox4_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox4); }
        private void SkillRGMinTBox5_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox5); }
        private void SkillRGMinTBox6_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox6); }
        private void SkillRGMinTBox7_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox7); }
        private void SkillRGMinTBox8_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox8); }
        private void SkillRGMinTBox9_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox9); }
        private void SkillRGMinTBox10_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox10); }
        private void SkillRGMinTBox11_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox11); }
        private void SkillRGMinTBox12_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox12); }
        private void SkillRGMinTBox13_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox13); }
        private void SkillRGMinTBox14_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox14); }
        private void SkillRGMinTBox15_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox15); }
        private void SkillRGMinTBox16_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox16); }
        private void SkillRGMinTBox17_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox17); }
        private void SkillRGMinTBox18_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox18); }
        private void SkillRGMinTBox19_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox19); }
        private void SkillRGMinTBox20_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMinTBox20); }

        private void SkillRGMaxTBox1_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox1); }
        private void SkillRGMaxTBox2_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox2); }
        private void SkillRGMaxTBox3_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox3); }
        private void SkillRGMaxTBox4_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox4); }
        private void SkillRGMaxTBox5_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox5); }
        private void SkillRGMaxTBox6_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox6); }
        private void SkillRGMaxTBox7_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox7); }
        private void SkillRGMaxTBox8_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox8); }
        private void SkillRGMaxTBox9_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox9); }
        private void SkillRGMaxTBox10_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox10); }
        private void SkillRGMaxTBox11_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox11); }
        private void SkillRGMaxTBox12_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox12); }
        private void SkillRGMaxTBox13_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox13); }
        private void SkillRGMaxTBox14_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox14); }
        private void SkillRGMaxTBox15_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox15); }
        private void SkillRGMaxTBox16_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox16); }
        private void SkillRGMaxTBox17_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox17); }
        private void SkillRGMaxTBox18_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox18); }
        private void SkillRGMaxTBox19_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox19); }
        private void SkillRGMaxTBox20_TextChanged(object sender, EventArgs e) { LimitTextBox(SkillRGMaxTBox20); }

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

        //checks keypresses to allow only numbers, plus "-" for negative values
        private void affects_textBox_value_KeyPress(object sender, KeyPressEventArgs e)     
        {
            TextBox textBox = (TextBox)sender;
            if (e.KeyChar == '-')
            {
                // If not at start of textbox or duplicate would result, ignore key
                if (textBox.SelectionStart != 0 || (textBox.Text.Length > 0 && textBox.Text[0] == '-' && (textBox.SelectionStart > 0 || textBox.SelectionLength == 0)))
                {
                    e.Handled = true;
                }
            }
            // Otherwise, if not Backspace or number key, ignore
            else if ((e.KeyChar != (char)Keys.Back && !char.IsDigit(e.KeyChar)))
            {
                e.Handled = true;
            }
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