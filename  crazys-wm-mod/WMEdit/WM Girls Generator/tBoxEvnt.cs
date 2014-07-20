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
        private void LimitTextBox(TextBox tbox)                                 //limits input to integers and limits it to 0 to 100 values, rough way, but it works, it would take quite some time to change it to different way (like the ones on bottom that have two events for every textbox, both keypress and textchanged, maybe I'll change it eventually)
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
                if (int.Parse(tbox.Text) > 101) tbox.Text = "100";
            }
        }

        private void LimitTextBoxRMin(TextBox tbox)
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
                if (int.Parse(tbox.Text) > 101) tbox.Text = "100";
            }
        }                          //I just realized that all 3 of these methods are the same, initially I planned to implement min/max boxes relations to these but that proved to be a bad idea, it was way to easy to enter endless loop, change to min box changes value in max box to keep them in correct relation, but that again activates change in max box and so on, ...

        private void LimitTextBoxRMax(TextBox tbox)
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
                if (int.Parse(tbox.Text) > 101) tbox.Text = "100";
            }
        }                          //in the end I implemented validate method that checks these relations all at once transparently, and also, users could pay some attention to what they enter in these boxes :P.

        public static bool IsNumeric(string strToCheck)                         //function to check if string is numeric or not
        {
            return Regex.IsMatch(strToCheck, "^\\d+(\\.\\d+)?$");
        }

        private void StatsTBox_01_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_01);
        }

        private void StatsTBox_02_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_02);
        }

        private void StatsTBox_03_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_03);
        }

        private void StatsTBox_04_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_04);
        }

        private void StatsTBox_05_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_05);
        }

        private void StatsTBox_06_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_06);
        }

        private void StatsTBox_07_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_07);
        }

        private void StatsTBox_08_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_08);
        }

        private void StatsTBox_09_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_09);
        }

        private void StatsTBox_10_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_G_Level);
        }

        private void StatsTBox_11_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_11);
        }

        private void StatsTBox_12_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_12);
        }

        private void StatsTBox_13_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_G_Exp);
        }

        private void StatsTBox_14_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_14);
        }

        private void StatsTBox_15_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_15);
        }

        private void StatsTBox_16_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_16);
        }

        private void StatsTBox_17_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_17);
        }

        private void StatsTBox_18_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_18);
        }

        private void StatsTBox_19_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_19);
        }

        private void StatsTBox_20_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_20);
        }

        private void StatsTBox_21_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_21);
        }

        private void StatsTBox_22_TextChanged(object sender, EventArgs e)
        {
            LimitTextBox(StatsTBox_22);
        }

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

        private void StatRGMinTBox1_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox1);
        }

        private void StatRGMinTBox2_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox2);
        }

        private void StatRGMinTBox3_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox3);
        }

        private void StatRGMinTBox4_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox4);
        }

        private void StatRGMinTBox5_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox5);
        }

        private void StatRGMinTBox6_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox6);
        }

        private void StatRGMinTBox7_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox7);
        }

        private void StatRGMinTBox8_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox8);
        }

        private void StatRGMinTBox9_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox9);
        }

        private void StatRGMinTBox10_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox10);
        }

        private void StatRGMinTBox11_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox11);
        }

        private void StatRGMinTBox12_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox12);
        }

        private void StatRGMinTBox13_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox13);
        }

        private void StatRGMinTBox14_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox14);
        }

        private void StatRGMinTBox15_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox15);
        }

        private void StatRGMinTBox16_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox16);
        }

        private void StatRGMinTBox17_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox17);
        }

        private void StatRGMinTBox18_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox18);
        }

        private void StatRGMinTBox19_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox19);
        }

        private void StatRGMinTBox20_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox20);
        }

        private void StatRGMinTBox21_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox21);
        }

        private void StatRGMinTBox22_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(StatRGMinTBox22);
        }

        private void StatRGMaxTBox1_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox1);
        }

        private void StatRGMaxTBox2_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox2);
        }

        private void StatRGMaxTBox3_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox3);
        }

        private void StatRGMaxTBox4_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox4);
        }

        private void StatRGMaxTBox5_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox5);
        }

        private void StatRGMaxTBox6_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox6);
        }

        private void StatRGMaxTBox7_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox7);
        }

        private void StatRGMaxTBox8_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox8);
        }

        private void StatRGMaxTBox9_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox9);
        }

        private void StatRGMaxTBox10_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox10);
        }

        private void StatRGMaxTBox11_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox11);
        }

        private void StatRGMaxTBox12_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox12);
        }

        private void StatRGMaxTBox13_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox13);
        }

        private void StatRGMaxTBox14_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox14);
        }

        private void StatRGMaxTBox15_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox15);
        }

        private void StatRGMaxTBox16_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox16);
        }

        private void StatRGMaxTBox17_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox17);
        }

        private void StatRGMaxTBox18_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox18);
        }

        private void StatRGMaxTBox19_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox19);
        }

        private void StatRGMaxTBox20_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox20);
        }

        private void StatRGMaxTBox21_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox21);
        }

        private void StatRGMaxTBox22_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(StatRGMaxTBox22);
        }

        private void SkillRGMinTBox2_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox2);
        }

        private void SkillRGMinTBox10_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox10);
        }
        private void SkillRGMinTBox11_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox11);
        }
        private void SkillRGMinTBox12_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox12);
        }
        private void SkillRGMinTBox13_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox13);
        }
        private void SkillRGMinTBox14_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox14);
        }
        private void SkillRGMinTBox15_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox15);
        }

        private void SkillRGMinTBox1_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox1);
        }

        private void SkillRGMinTBox3_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox3);
        }

        private void SkillRGMinTBox4_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox4);
        }

        private void SkillRGMinTBox5_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox5);
        }

        private void SkillRGMinTBox6_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox6);
        }

        private void SkillRGMinTBox7_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox7);
        }

        private void SkillRGMinTBox8_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox8);
        }

        private void SkillRGMinTBox9_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMin(SkillRGMinTBox9);
        }

        private void SkillRGMaxTBox2_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox2);
        }

        private void SkillRGMaxTBox10_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox10);
        }
        private void SkillRGMaxTBox11_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox11);
        }
        private void SkillRGMaxTBox12_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox12);
        }
        private void SkillRGMaxTBox13_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox13);
        }
        private void SkillRGMaxTBox14_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox14);
        }
        private void SkillRGMaxTBox15_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox15);
        }

        private void SkillRGMaxTBox1_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox1);
        }

        private void SkillRGMaxTBox3_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox3);
        }

        private void SkillRGMaxTBox4_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox4);
        }

        private void SkillRGMaxTBox5_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox5);
        }

        private void SkillRGMaxTBox6_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox6);
        }

        private void SkillRGMaxTBox7_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox7);
        }

        private void SkillRGMaxTBox8_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox8);
        }

        private void SkillRGMaxTBox9_TextChanged(object sender, EventArgs e)
        {
            LimitTextBoxRMax(SkillRGMaxTBox9);
        }

        private void GoldTBox1_KeyPress(object sender, KeyPressEventArgs e)                 //checks keypresses to allow only numbers
        {
            if ((e.KeyChar != (char)Keys.Back && !char.IsDigit(e.KeyChar)))
            {
                e.Handled = true;
            }
        }

        private void GoldTBox1_TextChanged(object sender, EventArgs e)                      //if everything in this textbox is deleted it puts 0 in it
        {
            if (GoldTBox1.Text.Length == 0) GoldTBox1.Text = "0";
        }

        private void affects_textBox_value_KeyPress(object sender, KeyPressEventArgs e)     //checks keypresses to allow only numbers, plus "-" for negative values
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