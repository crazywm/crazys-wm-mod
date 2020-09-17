/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "BarJobs.h"
#include "cGirls.h"
#include "Game.hpp"
#include "sStorage.h"
#include "buildings/IBuilding.h"
#include "buildings/cBuildingManager.h"
#include "character/sGirl.h"

bool cBarJob::DoWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    cGirls::UnequipCombat(girl);  // put that shit away, you'll scare off the customers!
    return JobProcessing(girl, *brothel, is_night);
}

cBarJob::cBarJob(JOBS job, const char* xml, sBarJobData data) : cBasicJob(job, xml), m_Data(data) {

}

void cBarJob::HandleGains(sGirl& girl, int enjoy, int jobperformance, int fame) {
    // update enjoyment
    girl.upd_Enjoyment(m_Data.Action, enjoy);

    if (girl.fame() < 10 && jobperformance >= 70)        { fame += 1; }
    if (girl.fame() < 20 && jobperformance >= 100)        { fame += 1; }
    if (girl.fame() < 40 && jobperformance >= 145)        { fame += 1; }
    if (girl.fame() < 60 && jobperformance >= 185)        { fame += 1; }

    girl.fame(fame);

    apply_gains(girl);
}

IGenericJob::eCheckWorkResult cBarJob::CheckWork(sGirl& girl, bool is_night) {
    auto brothel = girl.m_Building;
    if (girl.libido() >= 90 && girl.has_active_trait("Nymphomaniac") && chance(20))
    {
        ss << "${name} let lust get the better of her and she ended up missing her " << (is_night ? "night" : "day") << " shift.";
        girl.upd_temp_stat(STAT_LIBIDO, -20);
        girl.AddMessage(ss.str(), IMGTYPE_MAST, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    else if (girl.disobey_check(m_Data.Action, job()))
    {
        ss << m_Data.Refuse << " " << (is_night ? "tonight." : "today.");
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::REFUSES;
    }
    else if (brothel->m_TotalCustomers < 1)
    {
        ss << "There were no customers in the bar on the " << (is_night ? "night" : "day") << " shift so ${name} just cleaned up a bit.";
        brothel->m_Filthiness -= 20 + girl.service() * 2;
        girl.AddMessage(ss.str(), IMGTYPE_PROFILE, EVENT_NOWORK);
        return eCheckWorkResult::IMPOSSIBLE;
    }
    return eCheckWorkResult::ACCEPTS;
}

struct cBarCookJob : public cBarJob {
    cBarCookJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarCookJob::cBarCookJob() : cBarJob(JOB_BARCOOK, "BarCook.xml",
                                     {ACTION_WORKBAR, "${name} refused to cook food in the bar"}) {
}

bool cBarCookJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night)
{
    int roll_a = d100(), roll_b = d100();

    if (girl.has_active_trait("No Arms") || girl.has_active_trait("No Hands"))
    {
        ss << "You have assigned ${name} to work as a cook in the kitchens this week. She stares at the kitchen, with all the pots and pans and utensils, "
              "and then back at you, and then down at her own body, as if trying to draw your attention to the fact that she has no hands. "
              "After silently waiting for a response that you never give, "
              "she turns her gaze back to the kitchen and contemplates how she will even begin with this impossible task.\n \n";
    }
    else if (girl.has_active_trait("Retarded"))
    {
        ss << "${name} beams like a small child when you tell her that she is cooking today. "
              "You witness as she throws all the nearby ingredients into the same bowl, places the bowl on the stove, "
              "and then starts punching the mixture as if that is an approved cooking technique. \"I cook food good!\" she exclaims. "
              "Whatever possessed you to make a retarded girl into a cook, now you know that you are committed to seeing this through until the end.\n \n";
    }
    else if (girl.has_active_trait("Assassin"))// Impact unknown; randomly very bad, maybe?
    {
        ss << "${name}'s skills as an assassin have left her with a slightly skewed view towards food, which she casually refers to as \"poison masking.\" "
              "You are not sure whether you should have the waitresses serve what she cooks, "
              "and you definitely are not going to allow her to re-cook a meal that an angry customer sends back. "
              "Actually, there are a lot of knives in kitchens! You had not noticed that until right now. So many knives.\n \n";
    }
    else
    {
        ss << "${name} is assigned to work as a cook in the kitchen, preparing food for the customers.\n \n";
    }

    cGirls::UnequipCombat(girl);  // put that shit away, you'll scare off the customers!


    const sGirl* barmaidonduty = random_girl_on_job(brothel, JOB_BARMAID, is_night);
    std::string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->FullName() + "" : "the Barmaid");

    int wages = 15, tips = 0;
    int enjoy = 0, fame = 0;

    int imagetype = IMGTYPE_COOK;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;


#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_BARCOOK, false);

    //a little pre-game randomness
    if (chance(10))
    {
        if (girl.has_active_trait("Chef"))
        {
            ss << "${name} understands the difference between a \"Cook\" and a \"Chef.\" She ventures away from the recipe book occasionally, "
                  "spicing each dish to her own concept of perfection. It is usually a great improvement.\n";
            jobperformance += 15;
        }
        else if (girl.has_active_trait("Agile"))
        {
            ss << "${name} looks like she is handling a dozen pots and pans at the same time without any problems! She is certainly efficient.";
            jobperformance += 5;
        }
        else if (girl.has_active_trait("Mind Fucked"))
        {
            ss << "${name} works efficiently, but stares off into space with that vacant mindfucked expression of hers. "
                  "You would think there was nobody inside her brain at all, but sometimes, when she finishes cooking a dish, "
                  "she beams a maniacal grin and starts muttering about being a \"good girl.\" \"And good girls,\" she smiles as she jams the end of the spatula into her asshole, \"get dessert!\" "
                  "She giggles, removes the spatula, and then starts using it for the next dish. That cannot be hygienic.";
            jobperformance -= 10;
        }
        else if (girl.has_active_trait("One Hand"))
        {
            ss << "${name} struggles with only having one hand in the kitchen, but makes the best of it.";
            jobperformance -= 15;
        }
    }


    if (jobperformance >= 245)
    {
        ss << "  She must be the perfect at this.\n \n";
        wages += 155;

        if (girl.has_active_trait("No Arms") || girl.has_active_trait("No Hands"))
        {
            ss << "The food is incredible! But how?! How in hell did she do it without any hands? It just.. it just boggles your mind. "
                  "Maybe she carries the pots with her mouth? How does she plate the food? Honestly, this whole thing, while very impressive, "
                  "has left you with far more questions than answers.\n";
        }
        else if (roll_b >= 50)
        {
            if (girl.has_active_trait("Chef"))
            {
                ss << "${name} circulates briefly among the tables, talking to the patrons and asking their preferences so she can customize the menu for each of them. "
                      "They are amazed by her abilities, as she not only lovingly prepares each dish, but plates every morsel like an artist. "
                      "She circulates again after they eat, asking how they enjoyed their courses, and taking notes to improve them the next time. "
                      "Somehow, even with all this time out of the kitchen talking with the customers, she manages to ensure that each dish arrives just in time, "
                      "perfectly prepared and at the ideal temperature.\n";
                //tips here
            }
            else
            {
                ss << "She is an absolute frenzy of culinary activity, diving from plate to plate and stove to stove with expert grace. "
                      "The food always appears right on time, just the right temperature, and elegantly plated.\n";
            }
        }
        else
        {
            if (girl.has_active_trait("Retarded"))
            {
                ss << "There is no telling how she did it, whether by blind luck or through hidden idiot savant abilities, but ${name} actually made good food. "
                      "Great food, really. Truly surprisingly great food. She does not seem to understand the science of cooking, or which ingredients are which, "
                      "but somehow the final product is fit for any master chef. \"I did good cooking, yes?\" she exclaims to you before grabbing a turnip and repeatedly bashing it "
                      "against the table to \"make softer.\" While you have never seen this particular method of vegetable preparation before, you cannot argue with the results. "
                      "For reasons which should be obvious, you opt against introducing her to those customers that ask for the privilege of thanking the chef.\n";
                //tips here
            }
            else
            {
                ss << "She is a master chef, and she leaves the traditional menu far behind as she prepares individualized tasting plates for each patron. "
                      "${name} carefully prepares an appetizer of delicate quail eggs for one customer while drizzling rich chocolate on a raspberry tart for another. "
                      "You only receive high compliments from the patrons, who promise to come back for more.\n";
            }
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this.\n \n";
        wages += 95;

        if (girl.has_active_trait("No Arms") || girl.has_active_trait("No Hands"))
        {
            ss << "You are at a loss as to how she made such satisfying food without the use of her hands, "
                  "but hardly one to look a gift horse in the mouth, you accept it and move on.\n";
        }
        else if (roll_b >= 66)
        {
            ss << "${name} does very well today, and receives a number of compliments on her cooking. "
                  "She improves on the traditional menu in a number of small but appreciable ways, "
                  "perhaps by taking a minute to sear the meat just a bit longer, or chopping the vegetables into attractive florets. "
                  "All in all, there are certainly no complaints.\n";
        }
        else if (roll_b >= 33)
        {
            if (girl.has_active_trait("Chef"))
            {
                ss << "${name} circulates briefly among the tables, talking to the patrons and asking their preferences so she can customize the menu for each of them. "
                      "She considers this personal touch to be important, and while her skills are not sufficiently masterful to amaze each customer, "
                      "they are all satisfied with the results. She loses some time cooking while she makes the rounds, "
                      "meaning that some dishes arrive a few minutes later than would be ideal, but everyone seems to enjoy the personal touch.\n";
                //tips here
            }
            else
            {
                ss << "${name} attempts something completely new today. She places chicken meat into the cavity of a duck, "
                      "which she then stuffs into the cavity of a turkey, and then roasts all three together in some kind of perverse poultry centipede. "
                      "You have your doubts about this lunacy, but actually, it tastes delicious! You are not so sure about her name for it, \"Fowl Play,\" but the customers love it.\n";
            }
        }
        else
        {
            if (girl.has_active_trait("Retarded"))
            {
                ss << "Despite her mental limitations, ${name} manages to whip up some truly satisfying food. "
                      "Nobody knows how she does it, as she seems to just randomly throw various ingredients into each dish, but it serendipitously just works. "
                      "You have some reservations about her penchant to throw kale into everything, "
                      "and her repeated exclamations that it helps \"make poopy come on time\" for the customers are not overly persuasive. "
                      "Nevertheless, the food tastes really good, so why fight it?\n";
            }
            else
            {
                ss << "${name} shows her skills today with a seven-course tasting menu for the patrons, replete with saffron trout, "
                      "roasted beef joints, and bone-marrow soup. While you have enjoyed better food in your life, you have done so only rarely. "
                      "${name} is well on her way to becoming a master chef.\n";
            }
        }
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job.\n \n";
        wages += 55;

        if (girl.has_active_trait("No Arms") || girl.has_active_trait("No Hands"))
        {
            ss << "${name}'s final products are surprisingly acceptable, given her lack of hands, but you can say nothing more positive about the dishes. "
                  "Merely satisfactory. Perhaps the patrons would be more appreciative if they knew the herculean efforts that went into their food.\n";
        }
        else if (roll_b >= 66)
        {
            if (girl.has_active_trait("Retarded"))
            {
                ss << "For a retarded girl, ${name} is actually not that bad. Not great, and only bordering on good, "
                      "but at least not as bad as you would have anticipated. She tends to put meat in everything and has for no explicable reason forsworn all wheat products, "
                      "claiming that it is \"better for strong person eat like caveman,\" but this does not generate as many complaints as you would have imagined.\n";
            }
            else
            {
                ss << "${name} is an acceptable cook, and manages every item on the menu without disappointment. "
                      "She is not as fast as she could be, and some of the greens are occasionally wilted and the chicken rubbery, "
                      "but all in all, the customers are satisfied with her performance.\n";
            }
        }
        else if (roll_b >= 33)
        {
            ss << "${name} is still experimenting with some more advanced culinary concepts. The frog-leg milkshake is a dubious creation, "
                  "but surprisingly, the few customers with enough temerity to order it have largely positive reviews. "
                  "She is not a great cook yet, but with more practice, she very well could become one.\n";
        }
        else
        {
            if (girl.has_active_trait("Chef"))
            {
                ss << "${name} considers herself a chef, and tries to deviate from the menu with some additional spices or plating styles. "
                      "She clearly would like her efforts to be noticed, but with the final product being merely satisfactory, "
                      "nobody asks to meet and thank the cook. She resolves to try harder next time.\n";
            }
            else
            {
                ss << "${name} sticks to the menu today, showing no originality or culinary ambition. "
                      "Nevertheless, the food is satisfactory, timely, and generally well-received. "
                      "Perhaps ${name} will show more skill and experimentation as she becomes more comfortable with cooking.\n";
            }
        }
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;

        if (girl.has_active_trait("No Arms") || girl.has_active_trait("No Hands"))
        {
            ss << "${name} does about as well as you could hope for someone with no hands in a kitchen. "
                  "The food is acceptable enough to be served, and honestly, you are not sure you can ever expect any better than that from her.\n";
        }
        else if (roll_b >= 66)
        {
            if (girl.has_active_trait("Retarded"))
            {
                ss << "The results are better than you would expect, but only because the bar was so low. "
                      "This food is edible, and at times even acceptable, but not good. Someone at some point must have told ${name} that gluten is bad for people, "
                      "and since she has no idea what gluten is, this leads to some interesting additions and omissions in her meals. "
                      "The burger has a wheat bun, for instance, but she refused to use potatoes for the fries and opted instead for turnips. "
                      "You contemplate explaining to her what gluten is, but conclude that it would be a pointless effort.\n";
            }
            else
            {
                ss << "${name} is able to follow the routine recipe instructions, but only barely. She works step-by-step through each dish, "
                      "without imagination or experimentation, and produces acceptable results. She is a long way from a quiche, but at least she can fry onion rings.\n";
            }
        }
        else if (roll_b >= 33)
        {
            ss << "${name}'s cooking abilities are merely okay, and she knows it. She wisely avoids the more expensive ingredients, "
                  "knowing that she does not have the experience to cook them without ruining the dish, and focuses on the basics. "
                  "The customers enjoy some simple and basic fare. They may want something more refined, but if you can keep them sufficiently liquored up, "
                  "maybe they will not even notice.\n";
        }
        else
        {
            if (girl.has_active_trait("Chef"))
            {
                ss << "You are not sure what culinary academy decided to make ${name} a chef, "
                      "but you suspect that it was in a foreign country with radically different concepts of what constitutes edible food. "
                      "She is at best an acceptable cook, and her efforts at floretting vegetables and drizzling sauce reductions onto her plates "
                      "would be adorable if they were not so sad. Her efforts at difficult dishes are disasters, and most patrons wisely stick to simple pork chops.\n";
            }
            else {
                ss << "${name} tried something new, which is not a great idea for someone still mastering the basics of cooking. "
                      "The pork blood pudding she made was an ambitious undertaking, but when the customers saw the gelatinous mess, "
                      "they all opted for a simple hamburger. ${name} was disappointed, but resolved to try harder next time.\n";
            }
        }
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;

        if (girl.has_active_trait("No Arms") || girl.has_active_trait("No Hands"))
        {
            ss << "${name} heroically lifts a pot from the burner with her mouth, setting it to cook on the counter. "
                  "She bites down on a knife handle and bobs her head back and forth to cut the vegetables. "
                  "The end result is barely edible, but who would have the heart to tell her that?\n";
        }
        else if (roll_b >= 66)
        {
            ss << "The results are not good. ${name} may be able to boil water without setting the kitchen on fire, but those are the limits of her abilities. "
                  "She is able to produce some of the most simple items on the menu, like assembling the hamburgers, and is able to satisfy a few customers. "
                  "The remainder of her food is either sent back or thrown away.\n";
        }
        else if (roll_b >= 33)
        {
            if (girl.has_active_trait("Chef"))
            {
                ss << "Somebody, at some point, convinced ${name} that she was a chef. This person was a terrible liar. "
                      "She pours \"wine reductions,\" which is basically just slightly-heated wine from your cellars, all over dishes that do not benefit from her efforts. "
                      "Actually, they mostly just turn into a soupy mess. The patrons are not as upset as you would expect, though, "
                      "since ${name} did not heat the wine enough to burn away the alcohol, and they all essentially got a free drink.\n";
            }
            else
            {
                ss << "${name} wants to experiment with some new dishes for the menu, but her cooking is still so bad that every time she cooks could be considered an experiment. "
                      "Stewed pig's ears are a disaster. Nobody takes her up on her turnip dumplings. Maybe she will stumble onto an exciting new recipe one day, "
                      "but today is not that day.\n";
            }
        }
        else
        {
            if (girl.has_active_trait("Retarded"))
            {
                ss << "The results are bad. You had to know this when you assigned her. She is, after all, retarded, and combining ingredients based "
                      "solely on their colors is not a prudent way of cooking. Not that she cares, of course. This is just amazing fun for her. "
                      "You instruct the staff to keep the most expensive ingredients out of her reach, "
                      "because that kobe beef will almost certainly be ground together with ketchup and bananas into a barely palatable milkshake.\n";
            }
            else
            {
                ss << "${name} is not very good at this. She is terrified of half the items on the menu, which are well beyond her skills, "
                      "and barely scrapes by with the other half. The fries are soggy, the onion rings are basically raw, and the pork chops are cooked black. "
                      "She does not even attempt the more complicated dishes, either diverting those orders to other cooks or leaving those customers waiting.\n";
            }
        }
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;

        if (girl.has_active_trait("No Arms") || girl.has_active_trait("No Hands"))
        {
            ss << "The results are what you would expect for a girl who has to carry, cut, cook, and plate everything with her mouth. It is awful. "
                  "You consider telling the staff to 'give her a hand, but conclude that the remark would be considered in poor taste. "
                  "Not as poor taste as ${name}'s cooking, of course, but pretty close.\n";
        }
        else if (roll_b >= 66)
        {
            if (girl.has_active_trait("Retarded"))
            {
                ss << "The food is terrible, of course. Seriously. Of course it is. Did you know that she was retarded? Raw eggs and pork rinds do not mix well with coconut milk. "
                      "${name} had just a magical time throwing everything together, and she is smiling from ear to ear, but she is the only one. Honestly, what did you expect?\n";
            }
            else
            {
                ss << "Eyeing the aftermath, you doubt whether this could have gone much worse. "
                      "The kitchen is a disaster area, and pots and pans are strewn across the countertops with aborted efforts rotting inside. "
                      "The customers were repulsed by everything that she cooked, and even the dogs outside are turning up their noses at the massive piles you are forced to throw away. "
                      "This is not cooking, this is mad science.\n";
            }
        }
        else if (roll_b >= 33)
        {
            if (girl.has_active_trait("Chef"))
            {
                ss << "It seems that just about anybody can be called a \"chef\" these days. ${name}'s food edible the way that roots and leaves might be considered edible, "
                      "but nobody actually wants to order this trash. She painstakingly labored on floretting a baby tomato to place on top of a soupy, "
                      "vomit-like substance that she is calling a bisque, but the tomato ends up looking like a little mashed ketchup stain. Today could certainly have gone better.\n";
            }
            else
            {
                ss << "${name} is not a cook. She has no idea what she is doing. The stove baffles her, and the smoking blackened chunks of \"bread\" that come out baffle the customers. "
                      "A pot still lies on a burner, the water long since burned away, glowing orange and smelling like death. "
                      "She sheepishly turns off the burner and turns her head away from your gaze. This was an unmitigated disaster.\n";
            }
        }
        else
        {
            ss << "The kitchen did not burn down today. That is the only positive thing you can draw from the whole experience. "
                  "Customers were disgusted by everything ${name} cooked, if \"cooking\" is the right word for it, and not a few walked away shaking their heads. "
                  "Any profits were destroyed by all of the wasted ingredients. At least the rats will eat well tonight, "
                  "because ${name} has to throw away practically everything she attempted.\n";
        }
    }

    //try and add randomness here
    if (girl.has_active_trait("Clumsy") && chance(15))
    {
        ss << "Customers might wonder what that occasional cacophony of breaking glass coming from the kitchen is all about. "
              "Not you, though. You know that ${name} is slowly destroying your supply of dishes with her clumsiness. At least it was not another grease fire.\n";
        wages -= 15;
        jobperformance -= 10;
    }

    if (girl.has_active_trait("Homeless") && chance(15))
    {
        ss << "${name} has lived on the streets for so long that certain habits become unbreakable. "
              "When she is surrounded by food, for example, she usually cannot help but fill her pockets with leftovers and morsels that she will hoard for later. "
              "This may explain why each dish seems to need more ingredients than usual to prepare.\n";
        wages -= 25;
        jobperformance -= 5;
    }

    if (girl.has_active_trait("Blind"))
    {
        ss << "Some foodstuffs tend to feel the same way, and even smell the same, which is usually not a problem for a competent chef. "
              "${name} is blind, however, so she.. and also the customers.. may be surprised to find out what ends up in the food.\n";
        wages -= 10;
        jobperformance -= 10;
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift.";
        enjoy -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        enjoy += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //

    girl.AddMessage(ss.str(), imagetype, msgtype);

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    wages += uniform(10, 10 + roll_max);
    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    HandleGains(girl, enjoy, jobperformance, fame);

#pragma endregion
    return false;
}



struct cBarMaidJob : public cBarJob {
    cBarMaidJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarMaidJob::cBarMaidJob() : cBarJob(JOB_BARMAID, "BarMaid.xml",
                                     {ACTION_WORKBAR, "${name} refused to work as a barmaid in your bar"}) {
}

bool cBarMaidJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
#pragma region //    Job setup                //
    Action_Types actiontype = ACTION_WORKBAR;
    std::stringstream ss;
    int roll_jp = d100(), roll_e = d100(), roll_c = d100();
    ss << "${name} worked as a barmaid.\n \n";

    int wages = 0;
    double tips = 0;
    int enjoy = 0, fame = 0;                // girl
    int Bhappy = 0, Bfame = 0, Bfilth = 0;    // brothel
    int imagetype = IMGTYPE_WAIT;
    int msgtype = is_night;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_BARMAID, false);

    int numbarmaid = brothel.num_girls_on_job(JOB_BARMAID, is_night);
    int numbarwait = brothel.num_girls_on_job(JOB_WAITRESS, is_night);
    int numbargirls = numbarmaid + numbarwait;
    int numallcust = brothel.m_TotalCustomers;
    int numhercust = (numallcust / numbargirls)
                     + uniform(0, (girl.charisma() / 10) - 3)
                     + uniform(0, (girl.beauty() / 10) - 1);
    if (numhercust < 0) numhercust = 1;
    if (numhercust > numallcust) numhercust = numallcust;

    double drinkssold = 0;                                            // how many drinks she can sell in a shift
    for (int i = 0; i < numhercust; i++)
    {
        drinkssold += 1 + uniform(0, jobperformance / 30);    // 200jp can serve up to 7 drinks per customer
    }
    double drinkswasted = 0;                                        // for when she messes up an order

    if (chance(20))
    {
        if (actiontype >= 75)
        {
            ss << "Excited to get to work ${name} brings her 'A' game " << (is_night ? "tonight." : "today.");
            jobperformance += 40;
        }
        else if (actiontype <= 25)
        {
            ss << "The thought of working " << (is_night ? "tonight." : "today.") << " made ${name} feel uninspired so she didn't really try.";
            jobperformance -= 40;
        }
    }

    //what is she wearing?

    if (girl.has_item("Bourgeoise Gown") && chance(60))
    {
        int bg = rng().bell(-1, 1);
        roll_e += bg;                    // enjoy adj
        /* */if (bg < 0)    ss << "A few customers did not really like ${name}'s Bourgeoise Gown.";
        else if (bg > 0)    ss << "A few customers complimented ${name}'s Bourgeoise Gown.";
        else/*        */    ss << "${name}'s Bourgeoise Gown didn't really help or hurt her tips.";
        ss << "\n \n";
    }
    else if (girl.has_item("Maid Uniform"))
    {
        int bg = rng().bell(-1, 1);
        roll_e += bg;                    // enjoy adj
        /* */if (bg < 0)    ss << "A few customers teased ${name} for wearing a Maid's Uniform in a bar.";
        else if (bg > 0)    ss << "${name}'s Maid Uniform didn't do much for most of the patrons, but a few of them seemed to really like it.";
        else/*        */    ss << "${name}'s Maid Uniform didn't do much to help her.";
        ss << "\n \n";
    }

    //a little pre-game randomness
    if (girl.has_active_trait("Alcoholic"))
    {
        if (chance(10))
        {
            ss << "${name}'s alcoholic nature caused her to drink several bottles of booze becoming drunk and her serving suffered cause of it.";
            jobperformance -= 50;
            drinkswasted += uniform(10, 20);
        }
        ss << "\n \n";
    }

    if (jobperformance >= 245)
    {
        drinkssold *= 1.6;
        roll_e += 10;        // enjoy adj
        ss << "She must be the perfect bartender, customers go on and on about her and always come to see her when she works.\n";
        if (roll_jp <= 14)
        {
            ss << "${name} was sliding drinks all over the bar without spilling a drop she put on quite a show for the patrons.";
            Bfame += uniform(5, 10);
        }
        else if (roll_jp <= 28)
        {
            ss << "She agree to play a bar game with a client. Knowing every type of bar game there is, ${name} easily wins. The customer pays double for his drinks and leaves the bar saying that he will win next time.";
            wages += 25;
        }
        else if (roll_jp <= 42)
        {
            ss << "${name} made an 11 layer drink like it was nothing. The amazed customer left her a big tip!";
            Bfame += uniform(5, 10);
            tips += uniform(15, 40);
        }
        else if (roll_jp <= 56)
        {
            ss << "She pours eleven 100ml shots from a single, one litre bottle. Now there's a good barmaid!";
            drinkssold *= 1.1;
        }
        else if (roll_jp <= 70)
        {
            ss << "${name} noticed that a client was upset about something. After a pleasant conversation she managed to cheer him up. The client left full of willpower, leaving a generous tip behind.";
            Bhappy += uniform(5, 10);
            tips += uniform(15, 40);
        }
        else if (roll_jp <= 84)
        {
            ss << "Bottles fly high under the ceiling when ${name} is pouring drinks for the customers. The amazed crowd loudly applaudes every caught bottle and leave big tips for the girl.";
            Bfame += uniform(5, 10);
            tips += uniform(15, 40);
        }
        else
        {
            ss << "${name} mixed up what some patrons called the perfect drink.  It got them drunk faster then anything they had before.";
            Bhappy += uniform(5, 10);
            Bfame += uniform(5, 10);
        }
    }
    else if (jobperformance >= 185)
    {
        drinkssold *= 1.3;
        roll_e += 7;        // enjoy adj
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n";
        if (roll_jp <= 14)
        {
            ss << "${name} had the bar filled with happy drunks.  She didn't miss a beat all shift.";
            Bhappy += uniform(4, 8);
        }
        else if (roll_jp <= 28)
        {
            ss << "Today wasn't really exciting for ${name}. From boredom she spent some time trying to make more complicated drinks from the menu.";
        }
        else if (roll_jp <= 42)
        {
            ss << "${name} propose to a client to play a drinking game with her. If she loses she will serve nude to the end of her shift, "
                  "but if she wins he will be paying double. Some other patrons join the wager on the same terms. "
                  "After a few hours the last of them drops drunk and ${name} cleaned up on money.";
            wages += uniform(10, 30);
        }
        else if (roll_jp <= 56)
        {
            ss << "When taking orders from customers, ${name} talked them into buying more expensive drinks, that let you make a solid profit today.";
            wages += uniform(10, 30);
        }
        else if (roll_jp <= 70)
        {
            ss << "${name} is great at this job. At happy hour she was irreplaceable getting all the orders right. Later on she even prevented a fight between customers.";
            Bfame += uniform(4, 8);
        }
        else if (roll_jp <= 84)
        {
            ss << "Her shift was slow and hardly anyone was buying. ${name} took the initiative, announcing a special promotion.";
            int promo = 2;
            /* */if (girl.has_active_trait("Flat Chest")) promo = 1;
            else if (girl.has_active_trait("Petite Breasts")) promo = 1;
            else if (girl.has_active_trait("Small Boobs")) promo = chance(50) ? 1 : 2;
            else if (girl.has_active_trait("Busty Boobs")) promo = chance(80) ? 2 : 3;
            else if (girl.has_active_trait("Big Boobs")) promo = chance(70) ? 2 : 3;
            else if (girl.has_active_trait("Giant Juggs")) promo = chance(60) ? 2 : 3;
            else if (girl.has_active_trait("Massive Melons")) promo = chance(50) ? 2 : 3;
            else if (girl.has_active_trait("Abnormally Large Boobs")) promo = chance(40) ? 2 : 3;
            else if (girl.has_active_trait("Titanic Tits")) promo = chance(30) ? 2 : 3;
            if (promo == 1)
            {
                ss << "Every third shot ordered by a client could be drunk from her navel.\n";
            }
            else if (promo == 3)
            {
                ss << "Every pitcher ordered She would pour from between her breasts.\n";
            }
            else
            {
                ss << "Every third shot ordered by a client could be drunk from a shot-glass between her breasts.\n";
            }
            ss << "The promotion was such a success that you almost run out of booze.";
            drinkssold *= 1.2;
        }
        else
        {
            ss << "People love seeing ${name} work and they pour into the bar during her shift.  She mixes wonderful drinks and doesn't mess orders up so they couldn't be happier.";
            Bhappy += uniform(4, 8);
            Bfame += uniform(2, 6);
        }
    }
    else if (jobperformance >= 145)
    {
        drinkssold *= 1.1;
        roll_e += 3;        // enjoy adj
        ss << " She's good at this job and gets praised by the customers often.\n";
        if (roll_jp <= 14)
        {
            ss << "${name} didn't mix up any orders and kept the patrons drunk and happy.";
            Bhappy += uniform(2, 6);
        }
        else if (roll_jp <= 28)
        {
            ss << "${name} certainly knows what she is doing behind the bar counter. She spends her shift without making any mistakes and earning a lot from tips.";
            tips += uniform(5, 20);
        }
        else if (roll_jp <= 42)
        {
            ss << "${name} didn't make any mistakes today. She even earned some tips from happy customers.";
            Bhappy += uniform(2, 6);
            tips += uniform(5, 20);
        }
        else if (roll_jp <= 56)
        {
            ss << "When mixing one of the more complicated cocktails, ${name} noticed that she made a mistake and remakes the order. She wasted some alcohol, but the customer has happy with his drink.";
            Bhappy += uniform(2, 6);
            drinkswasted += 1;
        }
        else if (roll_jp <= 70)
        {
            ss << "${name} spent more time talking with customers than filling their glasses. She didn't earn much today.";
        }
        else if (roll_jp <= 84)
        {
            ss << "Her shift as a barmaid goes smoothly. ${name} feels more confident in her skills after today.";
        }
        else
        {
            ss << "${name} had some regulars come in.  She knows just how to keep them happy and spending gold.";
            Bhappy += uniform(2, 6);
        }
    }
    else if (jobperformance >= 100)
    {
        drinkssold *= 1.0;
        roll_e += 0;        // enjoy adj
        ss << " She made a few mistakes but overall she is okay at this.\n";
        if (roll_jp <= 14)
        {
            ss << "${name} mixed up a few drink orders...  But they ordered the same drink so it didn't matter too much.";
        }
        else if (roll_jp <= 28)
        {
            ss << "${name} made few mistakes but none of them were lethal.";
        }
        else if (roll_jp <= 42)
        {
            ss << "Trying her best she accidentally knocks down a bottle containing one of the bars most expensive liquors.";
            Bfilth += 5;
            drinkswasted += 10;
        }
        else if (roll_jp <= 56)
        {
            ss << "Maybe she isn't the best and fastest, but at least every drop of a drink that she pours stays in the glass.";
        }
        else if (roll_jp <= 70)
        {
            ss << "She agreed to play a bar game with a client. ${name} loses and the client spends the rest of his stay drinking on the house.";
            drinkswasted += 10;        // free drinks
        }
        else if (roll_jp <= 84)
        {
            ss << "${name} focused all her attention on taking orders and making drinks. Her attitude wasn't too appealing to clients. Some customers left feeling mistreated and unhappy.";
            Bhappy -= uniform(1, 5);
        }
        else
        {
            ss << "${name} wasted a few drinks by forgetting to put ice in them but it wasn't anything major.";
            drinkswasted += uniform(1, 5) % 5 + ((100 - girl.intelligence()) / 20);
        }
    }
    else if (jobperformance >= 70)
    {
        drinkssold *= 0.9;
        roll_e -= 3;        // enjoy adj
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n";
        if (roll_jp <= 14)
        {
            ss << "${name} mixed up people's drink orders...  When she only had four patrons drinking.";
            drinkswasted += uniform(1, 10) % 10 + ((100 - girl.intelligence()) / 10);
        }
        else if (roll_jp <= 28)
        {
            ss << "${name} is having a bad day and she isn't trying to hide it. Her bad attitude shows and rubs off on the customers, leaving a negative impression on them.";
            Bhappy -= uniform(2, 8);
        }
        else if (roll_jp <= 42)
        {
            ss << "Not being very good at this, she makes few mistakes. ${name} feels that she didn't improve today.";
            drinkswasted += uniform(1, 5);
        }
        else if (roll_jp <= 56)
        {
            ss << "Wanting to impress a client, she throws a bottle of an expensive liquor into the air. Trying to catch it behind her back, ${name} fails.";
            Bfilth += 5;
            drinkswasted += 10;
        }
        else if (roll_jp <= 70)
        {
            ss << "One patron, looking for encouragement or understanding from the barmaid, unfortunately approached ${name}. After a short conversation, he left crying.";
            Bhappy -= uniform(2, 6);
        }
        else if (roll_jp <= 84)
        {
            ss << "${name} tried to tap a new keg of beer; consequently she spends the rest of her shift mopping the floor.";
            Bfilth += 10;
            drinkswasted += uniform(10, 30);
        }
        else
        {
            ss << "${name} gave someone a drink she mixed that made them sick.  It was nothing but coke and ice so who knows how she did it.";
            Bhappy -= uniform(2, 6);
        }
    }
    else
    {
        drinkssold *= 0.8;
        roll_e -= 7;        // enjoy adj
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n";
        if (roll_jp <= 14)
        {
            ss << "${name} was giving orders to the wrong patrons and letting a lot people walk out without paying their tab.";
            drinkswasted += uniform(5, 20);
        }
        else if (roll_jp <= 28)
        {
            ss << "She mixed the ordered cocktails in the wrong proportions, making the clients throw up from the intoxication after just one shot! Besides swearing at her and yelling that they will never come here again, they left without paying.";
            Bhappy -= uniform(5, 10);
            Bfame -= uniform(3, 7);
            drinkswasted += uniform(5, 20);
        }
        else if (roll_jp <= 42)
        {
            ss << "You can see that standing behind the bar isn't her happy place. Being tense she made a lot of mistakes today.";
            Bhappy -= uniform(2, 6);
            drinkswasted += uniform(1, 15);
        }
        else if (roll_jp <= 56)
        {
            ss << "Not having any experience at this kind of job, ${name} tries her best.. Regrettably without results.";
            Bhappy -= uniform(2, 6);
            drinkswasted += uniform(1, 10);
        }
        else if (roll_jp <= 70)
        {
            ss << "She gets in a drinking game with a customer. Being a total lightweight, she gets drunk quickly and passes out on the job.";
            drinkssold *= 0.5;
        }
        else if (roll_jp <= 84)
        {
            ss << "She spends most of her shift flirting with one client and not paying much attention to the others. What's worse, the guy she was flirting with skips without paying the bill!";
            drinkssold *= 0.5;
            drinkswasted += uniform(1, 5);
        }
        else
        {
            ss << "${name} spilled drinks all over the place and mixed the wrong stuff when trying to make drinks for people.";
            Bhappy -= uniform(3, 8);
            drinkswasted += uniform(10, 20);
            Bfilth += 5;
        }
    }
    ss << "\n \n";

#pragma endregion
#pragma region    //    Tips and Adjustments        //

    tips += (drinkssold - drinkswasted) * ((double)roll_e / 100.0);    //base tips

    //try and add randomness here
    if (girl.beauty() > 85 && chance(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n";
        tips += 25;
    }

    if (girl.beauty() > 99 && chance(5))
    {
        tips += 50;
        ss << "${name} looked absolutely stunning during her shift and was unable to hide it. Instead of her ass or tits, the patrons couldn't take their eyes off her face, and spent a lot more than usual on tipping her.\n";
    }

    if (girl.charisma() > 85 && chance(20))
    {
        tips += 35;
        ss << "${name} surprised a couple of gentlemen discussing some complicated issue by her insightful comments when she was taking her order. They decided her words were worth a heavy tip.\n";
    }

    if (girl.intelligence() < 30 && chance(20))
    {
        ss << "${name} got confused when calculating the tabs, and seems to have damn near given away most of the alcohol.\n";
        drinkswasted += uniform(5, 30);
        Bhappy += 5;
    }

    if (girl.has_active_trait("Clumsy") && chance(15))
    {
        ss << "Her clumsy nature caused her to spill a drink on a customer resulting in them storming off without paying.\n";
        drinkswasted += uniform(1, 5);
    }

    if (girl.has_active_trait("Pessimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            ss << "Her pessimistic mood depressed the customers making them tip less.\n";
            tips -= 10;
        }
        else
        {
            ss << "${name} was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
            tips += 10;
        }
    }

    if (girl.has_active_trait("Optimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
            tips -= 10;
        }
        else
        {
            ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
            tips += 10;
        }
    }
    // `J` slightly lower percent compared to sleazy barmaid, I would think regular barmaid's uniform is less revealing
    if ((chance(3) && girl.has_active_trait("Busty Boobs")) ||
        (chance(6) && girl.has_active_trait("Big Boobs")) ||
        (chance(9) && girl.has_active_trait("Giant Juggs")) ||
        (chance(12) && girl.has_active_trait("Massive Melons")) ||
        (chance(16) && girl.has_active_trait("Abnormally Large Boobs")) ||
        (chance(20) && girl.has_active_trait("Titanic Tits")))
    {
        ss << "A patron was obviously staring at her large breasts. ";
        if (jobperformance < 150)
        {
            ss << "But she had no idea how to take advantage of it.\n";
        }
        else
        {
            ss << "So she over-charged them for drinks while they were too busy drooling to notice the price.\n";
            wages += 15;
        }
    }

    if (girl.has_active_trait("Psychic") && chance(20))
    {
        ss << "She used her Psychic skills to know exactly what the patrons wanted to order and when to refill their mugs, keeping them happy and increasing tips.\n";
        tips += 15;
    }

    if (girl.has_active_trait("Assassin") && chance(5))
    {
        if (jobperformance < 150)
        {
            ss << "A patron pissed her off and using her Assassin skills she killed him before she even realised. In the chaos that followed a number of patrons fled without paying.\n";
            drinkswasted += uniform(5, 35);    // customers flee without paying
            drinkssold /= 2;                    // customers don't come back
            wages -= 50;                        // pay off the victims family or officials to cover it up
            Bhappy -= 10;                        //
        }
        else
        {
            ss << "A patron pissed her off but she was able to keep her cool as she is getting used to this kinda thing.\n";
        }
    }

    if (girl.has_active_trait("Horrific Scars") && chance(15))
    {
        if (jobperformance < 150)
        {
            ss << "A patron gasped at her Horrific Scars making her sad. But they didn't feel sorry for her.\n";
        }
        else
        {
            ss << "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she did a wonderful job they left a good tip.\n";
            tips += 25;
        }
    }

    if (girl.morality() >= 80 && chance(20))
    {
        if (roll_jp <= 50)
        {
            tips += 35;
            ss << "During her shift ${name} spotted a depressed-looking lone man sinking his sorrows in alcohol. She spent a short while cheering him up. Surprised with her kindness, the client left her a generous tip.\n";
        }
        else
        {
            tips += 25;
            ss << "One of the patrons paid way too much for his order. When ${name} quickly pointed out his mistake, he said not to worry about it and told her to keep the extra as a reward for her honesty.\n";
        }
    }

    if (girl.morality() <= -20 && chance(20))
    {
        if (roll_jp <= 33)
        {
            tips += 35;
            ss << "During her shift, ${name} spotted a lone fellow passed out from alcohol alone at a table in a corner, his wallet bulging out of his pocket. Without a second thought, she discreetly snatched it out and claimed for herself.\n";
        }
        else if (roll_jp <= 66)
        {
            tips += 25;
            ss << "One of the patrons paid way too much for his order... and ${name} didn't really feel like pointing it out, considering the extra money a generous tip.\n";
        }
        else
        {
            tips -= 15;
            ss << "${name} responded to one of the vulgar remarks by a client in a much more vulgar way. Needless to say, this didn't earn her any favors with the patrons that shift, and her tips were a bit less than usual.\n";
        }
    }

    if (girl.morality() <= -20 && girl.dignity() <= -20 && chance(20))
    {
        tips += 40;
        ss << "A drunk patron suddenly walked up to ${name} and just started groping her body. Instead of pushing him away immediately, ${name} allowed him to take his time with her tits and butt while she helped herself to his pockets and all the money inside them. The rowdy client left with a dumb glee on his face, probably to find out his fondling was much, much overpriced.\n";
    }

    if (girl.dignity() <= -20 && chance(20))
    {
        if (roll_jp <= 50)
        {
            tips += 15;
            ss << "When taking an order, ${name} made sure to lean in really close for the client to get a full view of her cleavage. Giving him an eyefull of tits was promptly rewarded with some extra cash in tips.\n";
        }
        else
        {
            tips += 20;
            ss << "One of the rowdier clients gently slapped the butt of ${name} when she was passing through. Her coy giggle only encouraged more clients to occasionally fondle her butt through the rest of her work, which earned her some extra tips.\n";
        }
    }

    if (girl.dignity() <= -20 && chance(20) && (girl.has_active_trait("Big Boobs") ||
                                                     girl.has_active_trait("Abnormally Large Boobs") ||
                                                     girl.has_active_trait("Titanic Tits") ||
                                                     girl.has_active_trait("Massive Melons") ||
                                                     girl.has_active_trait("Giant Juggs")))
    {
        tips += 25;
        ss << "${name} got an odd request from a client to carry a small drink he ordered between her tits to his table. "
              "After pouring the drink in a thin glass, ${name} handled the task with minimal difficulty and earned a bigger tip.\n";
    }

    if (girl.morality() <= -20 && chance(10))
    {
        ss << "A patron came up to her and said he wanted to order some milk but that he wanted it straight from the source. ";
        if (girl.lactation() >= 20)
        {
            ss << "With a smile she said she was willing to do it for an extra charge. The patron quickly agreed and ${name} proceed to take out one of her tits and let the patron suck out some milk.\n";
            girl.lactation(-20);
            tips += 40;
        }
        else
        {
            ss << "She was willing to do it but didn't have enough milk production.";
        }
    }

    if (girl.is_pregnant() && chance(10))
    {
        ss << "A customer tried to buy ${name} a drink, but she refused for the sake of her unborn child.";
    }

    if ((girl.has_active_trait("Deep Throat") || girl.has_active_trait("No Gag Reflex")) && chance(5))
    {
        ss << "Some customers were having a speed drinking contest and challenged ${name} to take part.\n";
        if (girl.is_pregnant()) ss << "She refused for the sake of her unborn child.";
        else
        {
            ss << "Her talent at getting things down her throat meant she could pour the drink straight down. She won easily, earning quite a bit of gold.";
            tips += 30;
        }
    }

    if (girl.has_item("Golden Pendant") && chance(10))//zzzzz FIXME need more CRAZY
    {
        ss << "A patron complimented her gold necklace, you're not sure if it was an actual compliment or ";
        if (girl.has_active_trait("Massive Melons") || girl.has_active_trait("Abnormally Large Boobs")
            || girl.has_active_trait("Titanic Tits") || girl.has_active_trait("Big Boobs")
            || girl.has_active_trait("Busty Boobs") || girl.has_active_trait("Giant Juggs"))
        {
            ss << "an excuse to stare at her ample cleavage.";
        }
        else
        {
            ss << "an attempt to get a discount on their bill.";
        }
        girl.happiness(5);//girls like compliments
    }

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_e <= 10)
    {
        enjoy -= uniform(1, 3);
        drinkssold *= 0.9;
        ss << "\nSome of the patrons abused her during the shift.";
    }
    else if (roll_e >= 90)
    {
        drinkssold *= 1.1;
        enjoy += uniform(1, 3);
        ss << "\nShe had a pleasant time working.";
    }
    else
    {
        enjoy += uniform(0, 1);
        ss << "\nOtherwise, the shift passed uneventfully.";
    }
    ss << "\n \n";

    if (girl.is_unpaid())
        drinkssold *= 0.9;

#pragma endregion
#pragma region    //    Money                    //

    // drinks are sold for 3gp each, if there are not enough in stock they cost 1gp.
    int ds = std::max(0, (int)drinkssold);
    int dw = std::max(0, (int)drinkswasted);
    int d1 = ds + dw;                                                    // all drinks needed
    int d2 = g_Game->storage().drinks() >= d1 ? d1 : g_Game->storage().drinks();        // Drinks taken from stock
    int d3 = g_Game->storage().drinks() >= d1 ? 0 : d1 - g_Game->storage().drinks();    // Drinks needed to be bought
    int profit = (ds * 3) - (d3 * 1);
    g_Game->storage().add_to_drinks(-d2);
    if (profit < 0) profit = profit;
    else/*       */ profit = profit;
    if ((int)d1 > 0)
    {
        ss << "\n${name}";
        /* */if ((int)drinkssold <= 0)    ss << " didn't sell any drinks.";
        else if ((int)drinkssold == 1)    ss << " only sold one drink.";
        else/*                      */    ss << " sold " << ds << " drinks.";
        /* */if ((int)dw > 0)    ss << "\n" << dw << " were not paid for or were spilled.";
        /* */if (d2 > 0)/*           */ ss << "\n" << d2 << " drinks were taken from the bar's stock.";
        /* */if (d3 > 0)/*           */ ss << "\n" << d3 << " drinks had to be restocked durring the week at a cost of 1 gold each.";
        ss << "\n \n${name}";
        /* */if (profit > 0)/*       */    ss << " made you a profit of " << profit << " gold.";
        else if (profit < 0)/*       */    ss << " cost you " << profit << " gold.";
        else if (d1 > 0)/*           */ ss << " broke even for the week.";
        else/*                       */ ss << " made no money.";
    }

    if (girl.is_unpaid())
    {
        bool keep_tips = girl.keep_tips();
        if (!keep_tips)
        {
            wages += (int)tips;
            tips = 0;
        }
        /* */if ((int)wages > 0)    ss << "\n${name} turned in an extra " << (int)wages << " gold from other sources.";
        else if ((int)wages < 0)    ss << "\nShe cost you " << (int)wages << " gold from other sources.";
        if ((int)tips > 0 && keep_tips)
        {
            ss << "\nShe made " << (int)tips << " gold in tips";
            if ((int)wages < 0)
            {
                ss << " but you made her pay back what she could of the losses";
                int l = (int)tips + (int)wages;
                if (l > 0)        // she can pay it all
                {
                    tips -= l;
                    wages += l;
                }
                else
                {
                    wages += (int)tips;
                    tips = 0;
                }
            }
            ss << ".";
        }
        profit += (int)wages;    // all of it goes to the house
        wages = 0;
    }
    else
    {
        if (profit >= 10)    // base pay is 10 unless she makes less
        {
            ss << "\n \n"<< "${name} made the bar a profit so she gets paid 10 gold for the shift.";
            wages += 10;
            profit -= 10;
        }
        if (profit > 0)
        {
            int b = profit / 50;
            if (b > 0) ss << "\nShe gets 2% of the profit from her drink sales as a bonus totaling " << b << " gold.";
            wages += b;                    // 2% of profit from drinks sold
            profit -= b;
            girl.happiness(b / 5);
        }
        if (dw > 0)
        {
            girl.happiness(-(dw / 5));

            int c = std::min(dw, (int)wages);
            int d = std::min(dw - c, (int)tips);
            int e = std::min(0, dw - d);
            bool left = false;
            if (dw < (int)wages)                    // she pays for all wasted drinks out of wages
            {
                ss << "\nYou take 1 gold out of her pay for each drink she wasted ";
                wages -= c;
                profit += c;
                left = true;
            }
            else if (dw < (int)wages + (int)tips)    // she pays for all wasted drinks out of wages and tips
            {
                ss << "\nYou take 1 gold from her wages and tips for each drink she wasted ";
                wages -= c;
                tips -= d;
                profit += c + d;
                left = true;
            }
            else                                    // no pay plus she has to pay from her pocket
            {
                wages -= c;
                tips -= d;
                profit += c + d;
                if (girl.m_Money < 1)                // she can't pay so you scold her
                {
                    girl.pcfear(rng().bell(-1,5));
                    ss << "\nYou take all her wages and tips and then scold her for wasting so many drinks";
                }
                else if (girl.m_Money >= e)        // she has enough to pay it back
                {
                    girl.pcfear(rng().bell(-1, 2));
                    girl.pchate(rng().bell(-1, 2));
                    ss << "\nYou take all her wages and tips and then make her pay for the rest out of her own money";
                    girl.m_Money -= e;
                    profit += e;
                }
                else                                // she does not have all but can pay some
                {
                    girl.pcfear(rng().bell(-1, 4));
                    girl.pchate(rng().bell(-1, 2));
                    ss << "\nYou take all her wages and tips and then make her pay for what she can of the rest out of her own money";
                    e = girl.m_Money;
                    girl.m_Money -= e;
                    profit += e;
                }
            }

            if (left)
            {
                ss << "leaving her with ";
                /* */if ((int)wages + (int)tips < 1)    ss << "nothing";
                else if ((int)wages + (int)tips < 2)    ss << "just one gold";
                else/*                            */    ss << (int)wages + (int)tips << "gold";
            }
            ss << ".";
        }
    }



    // tiredness
#if 1    // `J` had some issues with tiredness so replacing it with a less complicated method until a better way is found 'Mute' Updated to fix math logic if this doesnt work feel free to switch back
    int t0 = d1;
    int easydrinks = (girl.constitution() + girl.service()) / 4;
    int t1 = easydrinks;                    // 1 tired per 20 drinks
    int t2 = std::max(0, t0 - t1);                // 1 tired per 10 drinks
    int t3 = std::max(0, t0 - (t1+t2));            // 1 tired per 2 drinks
    int tired = std::max(0,(t1/20))+std::max(0,(t2/10))+std::max(0,(t3/2));
#else
    int tired = max(1, (600 - ((int)jobperformance + (girl.constitution() * 3))) / 10);

#endif

    // Money
    girl.m_Tips = std::max(0, (int)tips);
    girl.m_Pay = std::max(0, wages);

    g_Game->gold().bar_income(profit);

#pragma endregion
#pragma region    //    Finish the shift            //

    brothel.m_Happiness += Bhappy;
    brothel.m_Fame += Bfame;
    brothel.m_Filthiness += Bfilth;

    girl.AddMessage(ss.str(), imagetype, msgtype ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);


    // Improve stats
    HandleGains(girl, enjoy, jobperformance, fame);

#pragma endregion
    return false;
}

struct cBarWaitressJob : public cBarJob {
    cBarWaitressJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarWaitressJob::cBarWaitressJob() : cBarJob(JOB_WAITRESS, "BarWaitress.xml",
                                             {ACTION_WORKBAR, "${name} refused to wait the bar"}) {
}

bool cBarWaitressJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_a = d100(), roll_b = d100(), roll_c = d100();
    ss << "${name} has been assigned to work as a waitress at your restaurant. She is informed that this is a genteel and conservative establishment, and she should focus on providing timely and efficient service.\n";
    if (girl.has_active_trait("Mind Fucked"))
    {
        ss << "${name} nods in understanding, but she also has a hand down her skirt, absent-mindedly rubbing her pussy as she listens. You are not entirely sure that she understands what \"genteel and conservative\" means here.. ${name}'s mind fucked state may make this a more interesting shift than you anticipated.\n \n";
    }
    else
    {
        ss << "${name} worked as a waitress in the bar.\n \n";
    }


    const sGirl* barmaidonduty = random_girl_on_job(*girl.m_Building, JOB_BARMAID, is_night);
    std::string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->FullName() : "the Barmaid");

    const sGirl* cookonduty = random_girl_on_job(*girl.m_Building, JOB_BARCOOK, is_night);
    std::string cookname = (cookonduty ? "Cook " + cookonduty->FullName() : "the cook");

    int wages = 15, tips = 0;
    int enjoy = 0, fame = 0;

    int imagetype = IMGTYPE_WAIT;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    int HateLove = girl.pclove() - girl.pchate();

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_WAITRESS, false);

    //a little pre-game randomness
    if (girl.has_active_trait("Mind Fucked") && chance(20))
    {
        ss << "${name} unnerves the customers with her far-away stare while she takes their orders, and causes a few gentlemen to sweat with discomfort as she casually mentions that she is also on the menu, if they would like to use her for anything. They all cough, turn their heads, and try to ignore it.";
        jobperformance -= 5;
    }
    if ((girl.breast_size() >= 5 || girl.has_active_trait("Sexy Air")) && chance(20))
    {
        ss << "Customers are willing to forgive a lot of mistakes where ${name} is concerned. Her sexy body distracts them when they order, and some find themselves ordering much more expensive wine than they had anticipated in an effort to impress her.";
        jobperformance += 5; tips += 10;
    }
    if ((girl.has_active_trait("Shy") || girl.has_active_trait("Nervous")) && chance(20))
    {
        ss << "Some customers are forced to flag ${name} down to get her to come to their table, as her nerves and shyness get in the way of providing suitable service. She is not comfortable talking with all of these new people.";
        jobperformance -= 10;
    }


    if (jobperformance >= 245)
    {
        ss << " She's the perfect waitress. Customers go on about her and many seem to come more for her than for the drinks or entertainment.\n \n";
        wages += 155;

        if (roll_b <= 14)
        {
            ss << "${name} danced around the bar dropping orders off as if she didn't even have to think about it.\n";
        }
        else if (roll_b <= 28)
        {
            ss << "Knowing how to speak with customers, ${name} always gets the clients to order something extra or more pricey then they wanted at the beginning, making you a tidy profit.\n";
            wages += 10;
        }
        else if (roll_b <= 42)
        {
            ss << "Being a very popular waitress made ${name} some fans, that come here only to be served by her. On the other hand they leave generous tips behind.\n";
            brothel.m_Fame += 10;
            tips += 10;
        }
        else if (roll_b <= 56)
        {
            ss << "Her shift past smoothly, earning her some really juicy tips.\n";
            tips += 15;
        }
        else if (roll_b <= 70)
        {
            ss << "Memorizing the whole menu and prices, taking out three or four trays at once, not making a single mistake for days. ${name} sure doesn't seem human.\n";
        }
        else if (roll_b <= 84)
        {
            ss << "Today she saved your place from a major disturbance. When greeting clients, ${name} noticed that the newly arrived group was part of a gang that was at war with another group of men which were already inside your place. She politely apologized and explained that your place was full and couldn't take such a large group. The men left unhappy but without giving her any trouble.\n";
        }
        else
        {
            ss << "People came in from everywhere to see ${name} work.  She bounces all around the bar laughing and keeping the patrons happy without messing anything up.\n";
            brothel.m_Happiness += 10;
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her work.\n \n";
        wages += 95;

        if (roll_b <= 16)
        {
            ss << "${name} bounced from table to table taking orders and recommending items to help you make more money.\n";
            wages += 10;
        }
        else if (roll_b <= 32)
        {
            ss << "She's very good at this. You saw her several times today carrying out two orders at a time.\n";
        }
        else if (roll_b <= 48)
        {
            ss << "When placing drink on the table she got slapped on the ass by one of the customers. ${name} scolded them in a flirty way, saying that this kind of behavior could get them kicked out by the security and that she didn't want to lose her favorite customers. The client apologies, assuring her that he didn't mean any harm.\n";
        }
        else if (roll_b <= 64)
        {
            ss << "Knowing the menu paid off for ${name}. After hearing the order she advised a more expensive option. Customers went for it and enjoyed their stay. Happy with the good advice they left her a great tip.\n";
            tips += 10;
        }
        else if (roll_b <= 83)
        {
            ss << "Memorizing the menu and prices has paid off for ${name}.\n";
        }
        else
        {
            ss << "${name} is a town favourite and knows most of the patrons by name and what they order.\n";
            brothel.m_Happiness += 5;
        }
    }
    else if (jobperformance >= 145)
    {
        ss << " She's good at this job and gets praised by the customers often.\n \n";
        wages += 55;

        if (roll_b <= 14)
        {
            // TODO this seems wrong -- fix
            if (girl.breast_size() >= 9)
            {
                ss << "The patrons love being served by ${name}.  Due to her skill at this job and the chance to stare at her legendary boobs.\n";
            } else if (girl.breast_size() >= 5)
            {
                ss << "The patrons love being served by ${name}.  Due to her skill at this job and the chance to stare at her Big Boobs.\n";
            }
            else
            {
                ss << "${name} didn't mess up any order this shift.  Keeping the patrons happy.\n";
                brothel.m_Happiness += 5;
            }
        }
        else if (roll_b <= 28)
        {
            ss << "One mishap today. A customer got away without paying!\n";
            wages -= 10;
        }
        else if (roll_b <= 42)
        {
            ss << "She spends her shift greeting customers and showing them to their tables.\n";
        }
        else if (roll_b <= 56)
        {
            ss << "Being confident in her skill, ${name} didn't make a single mistake today. She also earned some tips from happy customers.\n";
            tips += 10;
        }
        else if (roll_b <= 70)
        {
            ss << "Having a slow shift, she mostly gossip with other staff members.\n";
        }
        else if (roll_b <= 84)
        {
            ss << "Ensuring that every table was served, tired ${name} took a five minute breather.\n";
        }
        else
        {
            ss << "${name} had some regulars come in. She knows there order by heart and put it in as soon as she seen them walk in making them happy.\n";
            brothel.m_Happiness += 5;
        }
    }
    else if (jobperformance >= 100)
    {
        ss << " She made a few mistakes but overall she is okay at this.\n \n";
        wages += 15;

        if (roll_b <= 14)
        {
            ss << "${name} forgot to take an order to a table for over an hour.  But they were in a forgiving mood and stuck around.\n";
        }
        else if (roll_b <= 28)
        {
            ss << "Trying her best, ${name} made few mistakes but otherwise she had a pleasant day.\n";
            enjoy += 1;
        }
        else if (roll_b <= 42)
        {
            ss << "She tripped over her own shoelaces when carrying out an expensive order!\n";
            brothel.m_Filthiness += 5;
        }
        else if (roll_b <= 56)
        {
            ss << "Taking orders without mistakes and getting drinks to the tables not spilling a single drop from them. Today was a good day for ${name}.\n";
            enjoy += 1;
        }
        else if (roll_b <= 70)
        {
            ss << "When placing drink on the table she got slapped on the ass by one of the customers. ${name} didn't ignore that and called him names. Her behavior left the clients stunned for a moment.\n";
        }
        else if (roll_b <= 84)
        {
            ss << "${name} spent her shift on cleaning duty. Mopping the floor, wiping tables and ensuring that the bathrooms were accessible. She did a decent job.\n";
            brothel.m_Filthiness -= 5;
        }
        else
        {
            ss << "${name} sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
            brothel.m_Happiness -= 5;
        }
    }
    else if (jobperformance >= 70)
    {
        ss << " She was nervous and made a few mistakes. She isn't that good at this.\n \n";
        wages -= 5;
        if (roll_b <= 14)
        {
            ss << "${name} wrote down the wrong orders for a few patrons resulting in them leaving.\n";
            brothel.m_Happiness -= 5;
        }
        else if (roll_b <= 28)
        {
            ss << "After being asked for the fourth time to repeat his order, the irritated customer left your facility. ${name} clearly isn't very good at this job.\n";
            brothel.m_Happiness -= 5;
        }
        else if (roll_b <= 42)
        {
            ss << "Giving back change to a customer, ${name} made an error calculating the amount in favor of the client. \n";
            wages -= 5;
        }
        else if (roll_b <= 56)
        {
            ss << "Holding the tray firmly in her hands, ${name} successfully delivered the order to designated table. She was so nervous and focused on not failing this time, that she jumped scared when the customer thanked her.\n";
        }
        else if (roll_b <= 70)
        {
            ss << "${name} really hates this job. She used every opportunity to take a break.\n";
        }
        else if (roll_b <= 84)
        {
            ss << "Still learning to do her job, ${name} gets some orders wrong making a lot of people really angry.\n";
            brothel.m_Happiness -= 5;
        }
        else
        {
            ss << "${name} sneezed in an order she had just sat down on a table.  Needless to say the patron was mad and left.\n";
            brothel.m_Happiness -= 5;
        }
    }
    else
    {
        ss << " She was nervous and constantly making mistakes. She really isn't very good at this job.\n \n";
        wages -= 15;
        if (roll_b <= 14)
        {
            ss << "${name} was taking orders to the wrong tables and letting a lot of people walk out without paying their tab.\n";
        }
        else if (roll_b <= 28)
        {
            ss << "${name} was presenting the ordered dish when she sneezed in it. The outraged customer demanded a new serving, that he got on the house.\n";
            brothel.m_Happiness -= 5;
            wages -= 15;
        }
        else if (roll_b <= 42)
        {
            ss << "The tray slipped from ${name}'s hand right in front of the patron. Causing her to get yelled at for being bad at her job.\n";
            brothel.m_Filthiness += 5;
        }
        else if (roll_b <= 56)
        {
            ss << "Trying her best, ${name} focused on not screwing up today. Surprisingly she managed not to fail at one of her appointed tasks today.\n";
        }
        else if (roll_b <= 70)
        {
            ss << "After picking up a tray full of drinks from the bar, ${name} tried to bring it to the table. Her attempt failed when she slipped on wet floor that she mopped a minute ago.\n";
            brothel.m_Filthiness += 5;
        }
        else if (roll_b <= 84)
        {
            ss << "${name} was slacking on the job spending most of her shift chatting with other staff members.\n";
        }
        else
        {
            ss << "${name} spilled food all over the place and mixed orders up constantly.\n";
            brothel.m_Filthiness += 5;
        }
    }


    //base tips, aprox 10-20% of base wages
    tips += (int)(((10 + jobperformance / 22) * wages) / 100);

    //try and add randomness here
    if (girl.beauty() > 85 && chance(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n";
        tips += 25;
    }

    if (girl.has_active_trait("Clumsy") && chance(15))
    {
        ss << "Her clumsy nature cause her to spill food on a customer resulting in them storming off without paying.\n";
        wages -= 25;
    }

    if (girl.has_active_trait("Pessimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            ss << "Her pessimistic mood depressed the customers making them tip less.\n";
            tips -= 10;
        }
        else
        {
            ss << "${name} was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
            tips += 10;
        }
    }

    if (girl.has_active_trait("Optimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her services.\n";
            tips -= 10;
        }
        else
        {
            ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
            tips += 10;
        }
    }

    if (girl.has_active_trait("Psychic") && chance(20))
    {
        if (chance(50))
        {
            ss << "She used her Psychic skills to know exactly what the patrons wanted to order making them happy and increasing her tips.\n";
        }
        else
        {
            ss << "${name} uses her psychic abilities to her advantage. She knows when a customer is thinking about dessert or another bottle of wine, and shows up just in time with the perfect recommendation.\n";
        }
        tips += 20;
    }

    if (girl.has_active_trait("Great Arse") && chance(15))
    {
        if (jobperformance >= 185) //great
        {
            ss << "A patron reached out to grab her ass. But she skillfully avoided it with a laugh and told him that her ass wasn't on the menu.  He laughed so hard he increased her tip\n";
            tips += 15;
        }
        else if (jobperformance >= 135) //decent or good
        {
            ss << "A patron reached out and grabbed her ass. She's use to this and skilled enough so she didn't drop anything\n";
        }
        else if (jobperformance >= 85) //bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping half an order.\n";
            wages -= 10;
        }
        else  //very bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping a whole order\n";
            wages -= 15;
        }
    }

    if (girl.has_active_trait("Assassin") && chance(5))
    {
        if (jobperformance < 150)
        {
            ss << "A patron pissed her off and using her Assassin skills she killed him before even thinking about it resulting in patrons storming out without paying.\n";
            wages -= 50;
        }
        else
        {
            ss << "A patron pissed her off but she just gave them a death stare and walked away.\n";
        }
    }

    if (girl.has_active_trait("Horrific Scars") && chance(15))
    {
        if (jobperformance < 150)
        {
            ss << "A patron gasped at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n";
        }
        else
        {
            ss << "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she did a wonderful job they left a good tip.\n";
            tips += 25;
        }
    }

    if (girl.libido() > 90 && (girl.has_active_trait("Nymphomaniac") || girl.has_active_trait("Succubus") ||
                               girl.has_active_trait("Slut")))
    {
        ss << "During her shift, ${name} couldn't help but instinctively and excessively rub her ass against the crotches of the clients whenever she got the chance. Her slutty behavior earned her some extra tips, as a couple of patrons noticed her intentional butt grinding.\n";
        tips += 30;
    }

    if (chance(5))
    {
        /*if (girl.medicine() >= 90)
        { ss << "She used her Psychic skills to know excatally what the patrons wanted to order making them happy and increasing her tips.\n";
        wages += 15; }
        else if (girl.medicine() >= 60)
        { ss << "She used her Psychic skills to know excatally what the patrons wanted to order making them happy and increasing her tips.\n";
        wages += 15; }
        else*/ if (girl.medicine() >= 30)
        {
            ss << "A customer started chocking on his food so ${name} performed the heimlich maneuver on him. Grateful the man left her a better tip.\n";
            tips += 15;
        }
        else
        {
            ss << "A customer started chocking on his food so ${name} not knowing what to do started screaming for help.\n";
        }
    }

    if (girl.herbalism() >= 40 && chance(5))
    {
        ss << "Added a little something extra to the patrons order to spice it up. They enjoyed it greatly and she received some nice tips.\n \n";
        tips += 25;
    }

    if (brothel.num_girls_on_job( JOB_BARMAID, false) >= 1 && chance(25))
    {
        if (jobperformance < 125)
        {
            ss << "${name} wasn't good enough at her job to use " << barmaidname << " to her advantage.\n";
        }
        else
        {
            ss << "${name} used " << barmaidname << " to great effect speeding up her work and increasing her tips.\n";
            tips += 25;
        }
    }

    if (girl.has_active_trait("Fleet Of Foot") && chance(30))
    {
        ss << "${name} is fast on her feet, and makes great time navigating from table to table. She is able to serve almost twice as many customers in her shift.\n";
        tips += 50;
    }

    if (girl.has_active_trait("Dojikko") && chance(35))
    {
        ss << "${name} accidentally sends a tray of plates crashing to the floor, drawing the eyes of the entire restaurant to her. She smiles and sighs with such cuteness that everyone just laughs, and the customer whose dish splattered all over the tiles grins and says he would be happy to wait for a replacement, and that it could happen to anyone.\n";
        tips += 15;
        brothel.m_Happiness += 5;
    }

    //BIRTHDAY /**/
    if (girl.m_BDay >= 51)
    {
        if (girl.is_slave())
        {
        }
        else
        {
            if (HateLove >= 80)
            {
                if (girl.libido() >= 60)
                {
                    ss << "${name} has barely finished her shift before she is changed into her sexiest dress and standing before you. \"I have a little birthday wish,\" she whispers, drawing closer to you. \"I thought maybe, as your gift to me, you would allow me to serve you alone tonight. I asked " << cookname << " to cook all your favorite dishes, and I've prepared the upper dining area so it will just be the two of us.\" She leads you upstairs and seats you before disappearing for a moment and returning with the first course. ${name} feeds you with her own hands, giggling with every few bites. \"We have a cake, of course,\" she smiles as you finish everything, \"but that isn't the only dessert.\"\n";
                    if (roll_c >= 80)//ANAL
                    {
                        ss << "${name} lifts up her skirt so you can see that she is not wearing underwear. \"I was hoping that you might put your birthday present in my ass,\" she whispers into your ear, deftly opening your pants and lowering herself onto your suddenly and ragingly erect cock. She whimpers briefly as your dick penetrates her, then she spits on her hand and rubs the lubricant onto your tip before impaling herself again. \"You have no idea how often I fantasize about this when dealing with those stodgy customers all day,\" she pants, reveling as you ream her ass. \"Use me like a dirty backstreet whore,\" she continues, wrapping her asshole around you and bouncing up and down. It does not take long to cum for both of you. ${name} smiles";
                        ss << " with fulfillment as she lifts herself off your cock, semen leaking onto the table. \"I guess I'll need to clean that up,\" she comments, before turning back to you. \"Happy birthday to me,\" she grins. \"Let's do it again sometime.\"";
                        imagetype = IMGTYPE_ANAL;
                        girl.anal(1);
                    }
                    else if (roll_c >= 50)//SEX
                    {
                        ss << "${name} lies flat on her back on the cleared table, hiking up her dress so you have direct access to her wet pussy and placing the cake on her stomach. \"If you want dessert, I think you should come and get it,\" she purrs. You insert your hard-on into her and slowly fuck her as she moans, stopping only for a moment to take a piece of cake. You eat a bite and then feed her the rest as you pump with increasing speed, and as she takes the last bite, you spurt deep inside her. \"Happy birthday to me,\" she smiles.";
                        imagetype = IMGTYPE_SEX;
                        girl.normalsex(1);
                    }
                    else//ORAL
                    {
                        if (girl.oralsex() >= 50 && girl.has_active_trait("Deep Throat"))
                        {
                            ss << "${name} does not even wait for a reply before she moves her hand to your cock, deftly opening your pants and working you to a raging hard-on. She smiles mischievously at you and then dives down, swallowing your whole cock with one quick motion. She stays there, locked with her tongue on your balls and your shaft buried in her throat, massaging your cock with swallowing motions while staring with watering eyes into yours, until she begins to lose oxygen. You cum buckets straight down her throat as she begins to choke herself on you, and when she has secured every drop in her stomach, she pulls back, takes a deep breath, and smiles. \"Happy birthday to me,\" she says.";
                        }
                        else if (girl.oralsex() >= 50)
                        {
                            ss << "${name} kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is excellent, and you relish every moment, taking an occasional bite of cake as she latches onto your dick.";
                        }
                        else
                        {
                            ss << "${name} kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is not amazing, but it is delivered with such tenderness and love that you find yourself very satisfied regardless.";
                        }
                        imagetype = IMGTYPE_ORAL;
                        girl.oralsex(1);
                    }
                }
                else
                {
                    ss << "${name} finished her work and came to you with a shy grin. \"Did you know that it's my birthday?\" she asks, brushing against you gently and continuing without waiting for a response. \"I asked " << cookname << " to make a little something special, and I thought maybe we could share it together.\" The two of you enjoy a delicious light dinner, followed by an adorable little cake, as she giggles appreciably at your jokes and flirts with you. \"Maybe we should do this again without waiting a whole year,\" she suggests at the end of the evening, eyes flashing with unspoken promises. \"I'd love to thank you more fully for everything you have done for me.\"\n";
                }
            }
            else
            {
                ss << "${name} finished her work as a waitress and returned home for a private birthday celebration with her friends.\n";
            }
        }
    }


#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift.";
        enjoy -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working.";
        enjoy += 3;
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully.";
        enjoy += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //


    girl.AddMessage(ss.str(), imagetype, msgtype);

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    wages += uniform(10, 10+roll_max);
    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    HandleGains(girl, enjoy, jobperformance, fame);
    if (jobperformance > 150 && girl.constitution() > 65)
    {
        cGirls::PossiblyGainNewTrait(girl, "Fleet of Foot", 60, m_Data.Action, "${name} has been dodging between tables and avoiding running into customers for so long she has become Fleet of Foot.", is_night);
    }
    
#pragma endregion
    return false;
}

struct cBarPianoJob : public cBarJob {
    cBarPianoJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarPianoJob::cBarPianoJob() : cBarJob(JOB_PIANO, "BarPiano.xml",
                                       {ACTION_WORKMUSIC, "${name} refused to play piano in your bar"}) {
}

bool cBarPianoJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) {
    int roll_a = d100(), roll_b = d100();
    
    ss << "${name} played the piano in the bar.";

    const sGirl* singeronduty = random_girl_on_job(brothel, JOB_SINGER, is_night);
    std::string singername = (singeronduty ? "Singer " + singeronduty->FullName() + "" : "the Singer");

    int wages = 20, tips = 0;
    int enjoy = 0, fame = 0;
    int imagetype = IMGTYPE_PROFILE;
    auto msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_PIANO, false);
    tips = (int)((jobperformance / 8.0) * ((rng() % (girl.beauty() + girl.charisma()) / 20.0) + (girl.performance() / 5.0)));

    if (jobperformance >= 245)
    {
        ss << " She plays with the grace of an angel. Customers come from miles around to listen to her play.\n";
        roll_b += uniform(8, 30);                        // +8 to +30 enjoy check
        if (roll_a <= 20)
        {
            ss << "${name}'s playing brought many patrons to tears as she played a song full of sadness.";
            brothel.m_Happiness += uniform(5, 10);        // +5 to +10
        }
        else if (roll_a <= 40)
        {
            ss << "Nice melody fills the room when ${name} is behind the piano.";
            brothel.m_Happiness += uniform(5, 15);    // +5 to +15
        }
        else if (roll_a <= 60)
        {
            ss << "Knowing that she is good, ${name} played all the tunes blindfolded.";
            brothel.m_Fame += uniform(1, 10);            // +1 to +10
        }
        else if (roll_a <= 80)
        {
            ss << "Being confident in her skill, ${name} played today using only one hand.";
            brothel.m_Fame += uniform(1, 10);            // +1 to +10
        }
        else
        {
            ss << "${name}'s soothing playing seems to glide over the noise and bustling of the bar.";
            brothel.m_Happiness += uniform(5, 15);    // +5 to +15
        }
    }
    else if (jobperformance >= 185)
    {
        ss << " She's unbelievable at this and is always getting praised by the customers for her playing skills.\n";
        roll_b += uniform(4, 25);                        // +4 to +25 enjoy check
        if (roll_a <= 20)
        {
            ss << "${name} began to acquire her own following - a small crowd of people came in just to listen to her and buy drinks";
            brothel.m_Fame += uniform(1, 5);            // +1 to +5
            tips += uniform(10, 100);                    // +10 to +110
        }
        else if (roll_a <= 40)
        {
            ss << "Her playing fills the room. Some customers hum the melody under their noses.";
            brothel.m_Happiness += uniform(3, 10);        // +3 to +10
        }
        else if (roll_a <= 60)
        {
            ss << "After making a mistake she improvised a passage to the next song.";
        }
        else if (roll_a <= 80)
        {
            ss << "She plays without music sheets having all the songs memorized.";
            brothel.m_Fame += uniform(1, 5);            // +1 to +5
        }
        else
        {
            ss << "${name}'s soothing playing seems to glide over the noise and bustling of the bar.";
            brothel.m_Happiness += uniform(3, 10);        // +3 to +10
        }
    }
    else if (jobperformance >= 145)
    {
        ss << " Her playing is really good and gets praised by the customers often.\n";
        roll_b += uniform(-1, 15);                        //    -1 to +15 enjoy check
        if (roll_a <= 20)
        {
            ss << "Her playing was pleasing, if bland.  Her rythem was nice, if slightly untrained.";
        }
        else if (roll_a <= 40)
        {
            ss << "${name} doesn't have any trouble playing the piano.";
        }
        else if (roll_a <= 60)
        {
            ss << "Give ${name} any kind of music sheet and she will play it. She is really good at this.";
            brothel.m_Happiness += uniform(1, 5);        // +1 to +5
        }
        else if (roll_a <= 80)
        {
            ss << "When asked to play one of the more complicated tunes she gave her all.";
            brothel.m_Happiness += uniform(-1, 7);        // -1 to +5
        }
        else
        {
            ss << "The slow song ${name} played at the end of shift really had her full emotion and heart.";
        }
    }
    else if (jobperformance >= 100)
    {
        ss << " She hits a few right notes but she still has room to improve.\n";
        roll_b += uniform(-3, 10);                        // -3 to +10 enjoy check
        if (roll_a <= 20)
        {
            ss << "While she won't win any contests, ${name} isn't a terrible pianist.";
        }
        else if (roll_a <= 40)
        {
            ss << "${name}'s performance today was good. She is a promising pianist.";
        }
        else if (roll_a <= 60)
        {
            ss << "She gets the key order right most of the time.";
        }
        else if (roll_a <= 80)
        {
            ss << "You could tell that there was something like a melody, but ${name} still needs a lot of practice.";
        }
        else
        {
            ss << "The slow song ${name} played at the end of shift really had her full emotion and heart.  A pity that she felt so bored and tired.";
        }
    }
    else if (jobperformance >= 70)
    {
        ss << " She almost never hits a right note. Lucky for you most of the customers are too drunk and horny to care.\n";
        roll_b += uniform(-10, 5);                        // -10 to +5 enjoy check
        if (roll_a <= 20)
        {
            ss << "Her playing is barely acceptable, but fortunately the bustling of the bar drowns ${name} out for the most part.";
        }
        else if (roll_a <= 40)
        {
            ss << "She is terrible at this. Some customers left after she started to play.";
            brothel.m_Happiness -= uniform(1, 5);
        }
        else if (roll_a <= 60)
        {
            ss << "You could count on the fingers of one hand the part in her performance that was clean.";
            brothel.m_Happiness -= uniform(1, 5);
        }
        else if (roll_a <= 80)
        {
            ss << "She is bad at playing the piano.";
        }
        else
        {
            ss << "${name} knows a note.  Too bad it's the only one she knows and plays it over and over.";
        }
    }
    else
    {
        ss << " She didn't play the piano so much as banged on it.\n";
        roll_b += uniform(-15, 3);                        // -15 to +3 enjoy check
        if (roll_a <= 20)
        {
            ss << "Her audience seems paralyzed, as if they couldn't believe that a piano was capable of making such noise.";
            brothel.m_Happiness -= uniform(3, 10);        // -3 to -10
            brothel.m_Fame -= uniform(0, 3);            // 0 to -3
        }
        else if (roll_a <= 40)
        {
            ss << "After ten seconds you wanted to grab an axe and end the instrument's misery under ${name}'s attempt to play.";
            brothel.m_Happiness -= uniform(3, 10);        // -3 to -10
        }
        else if (roll_a <= 60)
        {
            ss << "Noone else would call this random key-mashing 'playing', but ${name} thinks otherwise.";
            brothel.m_Happiness -= uniform(3, 10);        // -3 to -10
        }
        else if (roll_a <= 80)
        {
            ss << "When ${name} started to play, the bar emptied almost instantly. This could be useful in a fire.";
            brothel.m_Fame -= uniform(1, 5);            // -1 to -5
        }
        else
        {
            ss << "${name} banged on the piano clearly having no clue which note was which.";
            brothel.m_Happiness -= uniform(3, 10);        // -3 to -10
        }
        //SIN - bit of randomness.
        if (chance(brothel.m_Filthiness / 50))
        {
            ss << "Soon after she started her set, some rats jumped out of the piano and fled the building. Patrons could be heard laughing.";
            brothel.m_Fame -= uniform(0, 1);            // 0 to -1
        }
    }
    ss << "\n \n";


    //try and add randomness here
    if (girl.beauty() > 85 && chance(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n";
        tips += uniform(15, 40);                    // +15 to +40
    }

    if (girl.has_active_trait("Clumsy") && chance(5))
    {
        ss << "Her clumsy nature caused her to close the lid on her fingers making her have to stop playing for a few hours.\n";
        wages -= 10;
        tips /= 2;
    }

    if (girl.has_active_trait("Pessimist") && chance(20))
    {
        if (jobperformance < 125)
        {
            ss << "Her pessimistic mood depressed the customers making them tip less.\n";
            tips = int(tips * 0.9);
        }
        else
        {
            ss << "${name} was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n";
            tips = int(tips * 1.1);
        }
    }
    else if (girl.has_active_trait("Optimist") && chance(10))
    {
        if (jobperformance < 125)
        {
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her on her playing.\n";
            tips = int(tips *0.9);
        }
        else
        {
            ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n";
            tips = int(tips * 1.1);
        }
    }

    if (girl.has_active_trait("Psychic") && chance(20))
    {
        ss << "She used her Psychic skills to know exactly what the patrons wanted to hear her play.\n";
        tips = int(tips * 1.1);
    }

    if (girl.has_active_trait("Assassin") && chance(5))
    {
        if (jobperformance < 150)
        {
            ss << "A patron bumped into the piano causing her to miss a note. This pissed her off and using her Assassin skills she killed him before even thinking about it, resulting in patrons fleeing the building.\n";
            wages = 0;
            tips = int(tips * 0.5);
        }
        else
        {
            ss << "A patron bumped into the piano, but with her skill she didn't miss a note. The patron was lucky to leave with his life.\n";
        }
    }

    if (girl.has_active_trait("Horrific Scars") && chance(15))
    {
        if (jobperformance < 150)
        {
            ss << "A patron gasped at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n";
        }
        else
        {
            ss << "A patron gasped at her Horrific Scars making her sad. Feeling bad about it as she played so well, they left a good tip.\n";
            tips = int(tips * 1.1);
        }
    }

    if (brothel.num_girls_on_job(JOB_SINGER, false) >= 1 && chance(25))
    {
        if (jobperformance < 125)
        {
            ss << "${name} played poorly with " << singername << " making people leave.\n";
            tips = int(tips * 0.8);
        }
        else
        {
            ss << "${name} played well with " << singername << " increasing tips.\n";
            tips = int(tips * 1.1);
        }
    }

#pragma endregion
#pragma region    //    Enjoyment                //

    //enjoyed the work or not
    if (roll_b <= 10)
    {
        ss << "Some of the patrons abused her during the shift.";
        enjoy -= uniform(1, 3);
        tips = int(tips * 0.9);
    }
    else if (roll_b >= 90)
    {
        ss << "She had a pleasant time working.";
        enjoy += uniform(1, 3);
        tips = int(tips * 1.1);
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        enjoy += uniform(0, 1);
    }

#pragma endregion
#pragma region    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        wages = 0;
        tips = int(tips * 0.9);
    }
    else
    {
    }

#pragma endregion
#pragma region    //    Finish the shift            //

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Update Enjoyment
    if (jobperformance < 50) enjoy -= 1;
    if (jobperformance < 0) enjoy -= 1;    // if she doesn't do well at the job, she enjoys it less
    if (jobperformance > 200) enjoy *= 2;        // if she is really good at the job, her enjoyment (positive or negative) is doubled

    // Base Improvement and trait modifiers
    HandleGains(girl, enjoy, jobperformance, fame);

    // Push out the turn report
    girl.AddMessage(ss.str(), imagetype, msgtype);
    return false;
}


struct cBarSingerJob : public cBarJob {
    cBarSingerJob();
    bool JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) override;
};

cBarSingerJob::cBarSingerJob() : cBarJob(JOB_SINGER, "BarSinger.xml",
                                         {ACTION_WORKMUSIC, "${name} refused to sing in your bar"}) {
}

bool cBarSingerJob::JobProcessing(sGirl& girl, IBuilding& brothel, bool is_night) 
{
    int roll_a = d100(), roll_b = d100();
    ss << "${name} worked as a singer in the bar.\n \n";

    int wages = 20, tips = 0;
    int enjoy = 0, happy = 0, fame = 0;
    int imagetype = IMGTYPE_SING;
    EventType msgtype = is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

#pragma endregion
#pragma region //    Job Performance            //

    double jobperformance = girl.job_performance(JOB_SINGER, false);

    const sGirl* pianoonduty = random_girl_on_job(*girl.m_Building, JOB_PIANO, is_night);
    std::string pianoname = (pianoonduty ? "Pianist " + pianoonduty->FullName() + "" : "the Pianist");

    //dont effect things but what she sings
    if (chance(60) && (girl.has_active_trait("Country Gal") || girl.has_active_trait("Farmers Daughter")))
        roll_a = 60;    // country
    else if (chance(60) && (girl.has_active_trait("Elegant")))
        roll_a = 30;    // classical
    else if (chance(60) && (girl.has_active_trait("Aggressive")))
        roll_a = 20;    // death metal
    else if (chance(60) && (girl.has_active_trait("Bimbo")))
        roll_a = 90;    // Pop Songs

    // `CRAZY` The type of music she sings
    std::string song_type_text = "Various types of music";
    /* */if (roll_a <= 10)    { song_type_text = "Goth Rock songs"; }
    else if (roll_a <= 20)    { song_type_text = "Death Metal songs"; }
    else if (roll_a <= 30)    { song_type_text = "Classical songs"; }
    else if (roll_a <= 40)    { song_type_text = "Metal songs"; }
    else if (roll_a <= 50)    { song_type_text = "Rock songs"; }
    else if (roll_a <= 60)    { song_type_text = "Country songs"; }
    else if (roll_a >= 90)    { song_type_text = "Pop songs"; }

    // `CRAZY` How well she sings
    /*default*/    std::string sing_pre_text;
    /* */if (jobperformance >= 245)    { sing_pre_text = " perfectly"; }
    else if (jobperformance >= 185)    { sing_pre_text = " great"; }
    else if (jobperformance >= 145)    { sing_pre_text = " good"; }
    else if (jobperformance >= 100)    { sing_pre_text = " decent"; }
    else if (jobperformance >= 70)    { sing_pre_text = " poorly"; }
    else                            { sing_pre_text = " very poorly"; }

    ss << "She sang " << song_type_text << sing_pre_text << ".\n";


    if (jobperformance >= 245)
    {
        ss << "She must have the voice of an angel, the customers go on and on about her and always come to listen when she sings.\n \n";
        wages += 155;
        if (roll_b <= 20)
        {
            ss << "${name}'s voice brought many patrons to tears as she sang a song full of sadness.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "Wanting to have some fun she encouraged some listeners to sing-along with her.\n";
            happy += 10;
        }
        else if (roll_b <= 60)
        {
            ss << "Feeling a little blue she only sang sad ballads today. You could swear that some customers were crying from the emotion.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "The whole room was quiet when ${name} was singing. After she finished, she gathered listeners applauded for minutes.\n";
            fame += 5;
        }
        else
        {
            ss << "${name}'s soothing voice seems to glide over the noise and bustling of the bar.\n";
        }
    }
    else if (jobperformance >= 185)
    {
        ss << "She's unbelievable at this and is always getting praised by the customers for her voice.\n \n";
        wages += 95;
        if (roll_b <= 25)
        {
            ss << "${name} begun to acquire her own following - a small crowd of people came in just to listen to her and buy drinks\n";
            fame += 10; wages += 10;
        }
        else if (roll_b <= 50)
        {
            ss << "Her performance was really great, giving the listeners a pleasant time.\n";
            happy += 5;
        }
        else if (roll_b <= 75)
        {
            ss << "When ${name} got on stage the crowd went wild. She didn't disappoint her fans giving one of the best performances in her life.\n";
            happy += 5;
        }
        else
        {
            ss << "${name}'s soothing voice seems to glide over the noise and bustling of the bar.\n";
        }
    }
    else if (jobperformance >= 145)
    {
        ss << "Her voice is really good and gets praised by the customers often.\n \n";
        wages += 55;
        if (roll_b <= 20)
        {
            ss << "Her singing was pleasing, if bland.  Her voice was nice, if slightly untrained.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "She sang every part of the song clearly. ${name} is a really good singer.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "This wasn't the best performance of her life time, but in general she did well.\n";
        }
        else if (roll_b <= 80)
        {
            ss << "She is good at this. With some work she could be a star.\n";
            fame += 5;
        }
        else
        {
            ss << "The slow song ${name} sang at the end of shift really had her full emotion and heart.\n";
        }
    }
    else if (jobperformance >= 100)
    {
        ss << "She hits a few right notes but she still has room to improve.\n \n";
        wages += 15;
        if (roll_b <= 25)
        {
            ss << "While she won't win any contests, ${name} isn't a terrible singer.\n";
        }
        else if (roll_b <= 50)
        {
            ss << "She didn't sing every part clearly but overall she was good.\n";
        }
        else if (roll_b <= 75)
        {
            ss << "Maybe she isn't the best but at least she doesn't scare away the customers.\n";
        }
        else
        {
            ss << "The slow song ${name} sang at the end of shift really had her full emotion and heart.  A pity she was bored and tired.\n";
        }
    }
    else if (jobperformance >= 70)
    {
        ss << "She almost never hits a right note. Lucky for you most of your customers are drunks.\n \n";
        wages -= 5;
        if (roll_b <= 20)
        {
            ss << "Her singing is barely acceptable, but fortunately the bustling of the bar drowns ${name} out for the most part.\n";
        }
        else if (roll_b <= 40)
        {
            ss << "After hearing today's performance, you order your guards to gag her for a week.\n";
        }
        else if (roll_b <= 60)
        {
            ss << "Some customers left immediately after ${name} started to sing.\n";
            wages -= 10;
        }
        else if (roll_b <= 80)
        {
            ss << "${name} singing was awful. Not a single line was sung clearly.\n";
            happy -= 5;
        }
        else
        {
            ss << "${name}'s voice is all over the place as she sings.\n";
        }
    }
    else
    {
        ss << "Her voice sounds like nails on a chalk board.  She could be the worst singer ever.\n \n";
        wages -= 15;
        if (roll_b <= 14)
        {
            ss << "Her audience seems paralyzed, as if they couldn't believe that a human body could produce those sounds, much less call them \"singing\".\n";
            happy -= 5;
        }
        else if (roll_b <= 28)
        {
            ss << "It's tragic, ${name} really can't sing.\n";
        }
        else if (roll_b <= 42)
        {
            ss << "She is just terrible. You could swear that your singing under the shower is far better.\n";
        }
        else if (roll_b <= 56)
        {
            ss << "${name} was the first to get on stage before today's star performance. Seeing the gathered crowd, she froze up being able to let out a single word.\n";
        }
        else if (roll_b <= 70)
        {
            ss << "${name} singing was awful. Angry listeners throw random objects at her.\n";
            happy -= 5;
        }
        else if (roll_b <= 84)
        {
            ss << "Hearing ${name}'s singing gave you a thought to use it as a new torture method.\n";
        }
        else
        {
            ss << "${name} bellowed out a melody that caused the bar to go into a panic clearing it quickly.\n";
            happy -= 10;
        }
    }

    //base tips, aprox 5-30% of base wages
    tips += (int)(((5 + jobperformance / 8) * wages) / 100);

    //try and add randomness here
    if (girl.beauty() >85 && chance(20))
    {
        ss << "Stunned by her beauty a customer left her a great tip.\n \n"; tips += 15;
    }

    if (girl.charisma() > 80 && chance(15))
    {
        ss << "Her charisma shone through as she chatted to customers between songs.\n \n"; tips += 15; happy += 5;
    }

    if (girl.has_active_trait("Clumsy") && chance(5))
    {
        ss << "Her clumsy nature caused her to trip coming on stage causing the crowed to go wild with laughter. She went to the back and hide refusing to take the stage for her song set.\n"; wages -= 15;
    }

    if (girl.has_active_trait("Pessimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            ss << "Her pessimistic mood depressed the customers making them tip less.\n"; tips -= 10;
        }
        else
        {
            ss << "${name} was in a poor mood so the patrons gave her a bigger tip to try and cheer her up.\n"; tips += 10;
        }
    }

    if (girl.has_active_trait("Optimist") && chance(5))
    {
        if (jobperformance < 125)
        {
            ss << "${name} was in a cheerful mood but the patrons thought she needed to work more on her on her singing.\n"; tips -= 10;
        }
        else
        {
            ss << "Her optimistic mood made patrons cheer up increasing the amount they tip.\n"; tips += 10;
        }
    }

    if (girl.has_active_trait("Psychic") && chance(20))
    {
        ss << "She knew just what songs to sing to get better tips by using her Psychic powers.\n"; tips += 15;
    }

    if (girl.has_active_trait("Assassin") && chance(5))
    {
        if (jobperformance < 150)
        {
            ss << "A patron booed her. She saw red and using her Assassin skills killed him instantly. Other patrons fled out without paying.\n"; wages -= 50;
        }
        else
        {
            ss << "A patron booed her. But was drunk and started crying a moment later so she ignored them.\n";
        }
    }

    if (girl.has_active_trait("Horrific Scars") && chance(15))
    {
        if (jobperformance < 150)
        {
            ss << "A patron gasped and pointed at her Horrific Scars making her uneasy. But they didn't feel sorry for her.\n";
        }
        else
        {
            ss << "A patron gasped and pointed at her Horrific Scars making her sad. Her singing was so wonderful that at the end of the performance they personally apologized and thanked her, leaving her a good tip.\n"; tips += 15;
        }
    }

    if (girl.has_active_trait("Idol") && girl.fame() > 75 && chance(25))
    {
        ss << "Today a large group of ${name}'s followers came to listen to her sing, leaving very generous tips behind.\n";
        wages += 15;
        tips += 25 + girl.fame() / 4;
        girl.fame(1);
    }
    else if (girl.has_active_trait("Idol") && chance(25))
    {
        ss << "A group of ${name}'s fans came to listen to her sing, leaving good tips behind.\n";
        wages += 10;
        tips += 20 + girl.fame() / 5;
        girl.fame(1);
    }
    else if (!girl.has_active_trait("Idol") && girl.fame() > 75 && chance(15))
    {
        ss << "${name} is quite popular in Crossgate so a small crowd of people came in just to listen to her.\n";
        wages += 5;
        tips += 15;
        girl.fame(1);
    }

    if (brothel.num_girls_on_job(JOB_PIANO, is_night) >= 1 && chance(25))
    {
        if (jobperformance < 125)
        {
            ss << "${name}'s singing was out of tune with " << pianoname << " causing customers to leave with their fingers in their ears.\n"; tips -= 10;
        }
        else
        {
            ss << pianoname << " took her singing to the next level causing the tips to flood in.\n"; tips += 40;
        }
    }

#pragma endregion
#pragma region    //    Enjoyment and Tiredness        //

    //enjoyed the work or not
    if (roll_a <= 5)
    {
        ss << "\nSome of the patrons abused her during the shift."; enjoy -= 1;
    }
    else if (roll_a <= 25)
    {
        ss << "\nShe had a pleasant time working."; enjoy += 3; girl.confidence(1);
    }
    else
    {
        ss << "\nOtherwise, the shift passed uneventfully."; enjoy += 1;
    }

#pragma endregion
#pragma region    //    Money                    //


#pragma endregion
#pragma region    //    Finish the shift            //

    brothel.m_Fame += fame;
    brothel.m_Happiness += happy;
    
    girl.AddMessage(ss.str(), imagetype, msgtype);
    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    wages += uniform(10, 10 + roll_max);

    // Money
    girl.m_Tips = std::max(0, tips);
    girl.m_Pay = std::max(0, wages);

    // Improve stats
    HandleGains(girl, enjoy, jobperformance, fame);
    if (girl.fame() >= 70 && chance(10))
    {
        cGirls::PossiblyGainNewTrait(girl, "Idol", 50, m_Data.Action, "Her fame and singing skills has made ${name} an Idol in Crossgate.", is_night);
    }


#pragma endregion
    return false;
}

void RegisterBarJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cBarCookJob>());
    mgr.register_job(std::make_unique<cBarMaidJob>());
    mgr.register_job(std::make_unique<cBarWaitressJob>());
    mgr.register_job(std::make_unique<cBarPianoJob>());
    mgr.register_job(std::make_unique<cBarSingerJob>());
}

