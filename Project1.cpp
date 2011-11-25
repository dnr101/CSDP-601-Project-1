/*
10/10/08 The program is fully functional in it's semi original state. I have added a
number of system calls to "cls" to make the display more attractive. The data file is
still called "junk.dat" and the program will look for it on the D: drive in a folder called cppSpace.
This can  easily be modified below, but it does need to have access to a file for storage for
it to function as expected. I will work on adding the BSTree functionality this weekend.
Email me when you get the dynamic memory working. -Dave

10/11/08 I started implementing the dict class as a BS Tree. -Dave

10/11/08 BSTree implemented and compiles, howerver, it produces run-time errors. Please
check my code to see if there is something I am missing. Thanks -Dave
*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctype.h>

using namespace std;
const int VSIZE = 5;

const char OURDATAFILE[] = "d:\\cppSpace\\junk.dat";
//Classes Needed
class wDPair
{
    public:
        wDPair(const char * = NULL, const char * = NULL);   //word, def
        //both null constructor (with defalut vals) and conversion constructor
        wDPair(const wDPair &);                     //copy constr
        ~wDPair();                                  //destructor
        wDPair & operator= (const wDPair &);        //assignment oper
        const char* tellWord() const;
        const char* tellDef() const;
        void changeWord(const char*);
        void changeDef(const char*);
    private:
        char theWord[15];
        char theDef[25];
        wDPair *p;          //added pointers to implement BST
        wDPair *left;
        wDPair *right;
    friend class dict;      //So that the tree and nodes can access each other's data...
};

class dict                          //most work converting to RB tree will go here...
{
    public:
        dict(char* = NULL);         //file name
        ~dict();
        wDPair* findWord(const char*);
        const wDPair* findDef(const char*);
        bool insertPair(wDPair*);
        bool deletePair(const char*);
        bool empty()const;
        bool full() const;
        int tellSize() const;
        const wDPair* tellFirst();      //part of 'built in' iterator
        const wDPair* tellNext();       //part two of iterator
        bool diskSave(const char*);     //file name - false on fail
        bool diskRecover(const char*);
        wDPair * nextPair(wDPair*);     //Non-constant next function used in the deletePair function.
    protected:
        //wDPair theDic[VSIZE];           //10-10 changing this to a BS Tree...
        wDPair* nil;                       //pointer to Nil node
        wDPair* root;                     //pointer to root
        int currentSize;
        wDPair* currentLoc;                 //Iterator's location
    friend class wDPair;                //see above...
};


class menu          //no changes here from hand-out
{
    public:
        menu(int = 0, char** = NULL, void(**)(void) = NULL);        //size, "words', funcs
        ~menu();
        void display(void)const;
        char getChoice(void);
        bool done(void)const;
        void doChoice(char &)const;
        void typicalQuit(void);
    protected:
        int numItems;
        char** menuItems;
        void(** theChoices)(void);
        char theChoice;

};

//MENU FUNC DECS
void doAdd(void);
void doDelete(void);
void doFind(void);
void doList(void);
void doQuit(void);

//MAIN PROG FUNC DECS
void initialize1(void);
void terminate1(void);              //save data before exit

//MAKING A MENU AND A DICTIONARY
char* menuWords[] = {"Add Entry", "Delete Entry", "Find Entry", "List Entrys", "Quit"};
void(* menuChoices[])(void) = {doAdd, doDelete, doFind, doList, doQuit};
menu * theMenu = new menu(5, menuWords, menuChoices);
dict* myDic = new dict;

int main(void)
{
    initialize1();
    char choiceIn = '0';
    do
    {
        theMenu->display();
        choiceIn = theMenu->getChoice();
        theMenu->doChoice(choiceIn);
    }while(!(theMenu->done()));
    terminate1();
};

wDPair::wDPair(const char* newWord, const char* newDef)
{
    cout << "New wDPair constructed..."<<endl;
    if (newWord != NULL)
    {
        strcpy(theWord, newWord);
    }
    else
    {
        theWord[0] = 0;
    };
    if (newDef != NULL)
    {
        strcpy(theDef, newDef);
    }
    else
    {
        theDef[0] = 0;
    };
    this->p = this->left = this->right = NULL;
};


wDPair::wDPair(const wDPair & otherPair)
{
    strcpy(theWord, otherPair.theWord);
    strcpy(theDef, otherPair.theDef);
};

wDPair::~wDPair()
{
    //Somehow delete pair...
};

wDPair & wDPair::operator= (const wDPair & otherPair)
{
    if (this != & otherPair)
    {
        strcpy(theWord, otherPair.theWord);
        strcpy(theDef, otherPair.theDef);
        this->p = otherPair.p;                   //Must copy a node's pointers as well as it's data...
        this->left = otherPair.left;
        this->right = otherPair.right;
    };
    return * this;
};

const char * wDPair::tellWord()const
{
    return theWord;
};

const char * wDPair::tellDef()const
{
    return theDef;
};

void wDPair::changeWord(const char * newWord)
{
    strcpy(theWord, newWord);
};

void wDPair::changeDef(const char * newDef)
{
    strcpy(theDef, newDef);
};



//Details for the Class dict...
//Many changes here to implement BSTree
dict::dict(char* fileName)
{
    cout << "New dict..."<<endl;
    currentSize = 0;
    if(fileName != NULL)
    {
        diskRecover(fileName);
    }
    else
    {
        nil = new wDPair();
        root = NULL;            //set the root initially to nil
        currentLoc = root;
    };

};

dict::~dict()
{
    //nothing to do... yet?
};

wDPair * dict::findWord(const char * searchWord)
{
//    old version...
//    for(int i = 0; i < currentSize; i++)
//    {
//        if (!strcmp(theDic[i].tellWord(), searchWord))
//        {
//            return & theDic[i];
//        };
//    };
//    return NULL;
    cout << "findWord..."<<endl;
    wDPair *x = root;
    cout << "*x = root..."<<endl;
    if (root = nil) return NULL;
    while ((x != nil) && (searchWord != x->theWord))
    {
        cout << "(x != nil) && (searchWord != x->theWord)"<<endl;
        if (searchWord < x->theWord)
        {
            cout << "searchWord < x->theWord"<<endl;
            x = x->left;
        }
        else
        {
            cout << "searchWord > x->theWord"<<endl;
            x = x->right;
        };
    };
    if (x->theWord[0] == 0)
    {
        return NULL;
    }
    else
    {
        return x;
    };
};

const wDPair* dict::findDef(const char* searchDef)
{
    return NULL;
//    old version...
//    for (int i = 0; i < currentSize; i++)
//    {
//        if (!strcmp(theDic[i].tellDef(), searchDef))
//        {
//            return & theDic[i];
//        };
//    };
//    return NULL;
//    wDPair *x = root;
//    while ((x != nil) && (strcmp(searchDef, x->theDef) != 0))
//    {
//        if (searchDef < x->theDef)
//        {
//            x = x->left;
//        }
//        else
//        {
//            x = x->right;
//        };
//    };
//    if (x->theDef[0] == '?')
//    {
//        return NULL;
//    }
//    else
//    {
//        return x;
//    };
};


bool dict::insertPair(wDPair * newPair)
{
    cout<<"insertPair..."<<endl;
    if(currentSize == VSIZE)
    {
        return false;
    }
    else
    {
//    old version...
//        int i = 0, j;
//        while ((i < currentSize)&&(strcmp(theDic[i].tellWord(), newPair->tellWord()) < 0))
//        {
//            i++;
//        };
//        if (i < currentSize)
//        {
//            for (j = currentSize; j > i; j--)
//            {
//                theDic[j] = theDic[j - 1];
//            };
//        };
//        theDic[i] = *newPair;
//        currentSize++;
//        return true;
        wDPair *y = NULL;
        cout<<"wDPair *y = NULL;";
        wDPair *x = root;
        cout<<"wDPair *x = root;"<<endl;
        if (root == nil)
        {
            cout<<"root == nil"<<endl;
            root = newPair;
            newPair->p = newPair->left = newPair->right = nil;
        }
        else
        {
            cout<<"root!=nil"<<endl;
            while (x != nil)
            {
                y = x;
                if (newPair->theWord < x->theWord)
                {
                    x = x->left;
                }
                else
                {
                    x = x->right;
                };
            };
            newPair->p = y;
            newPair->left = newPair->right = nil;
            if (newPair->theWord < y->theWord)
            {
                y->left = newPair;
            }
            else
            {
                y->right = newPair;
            };
            currentSize++;
            return true;
        };
    };
};

bool dict::deletePair(const char* oldWord)
{
    cout<<"deletePair..."<<endl;
    if (currentSize == 0)
    {
        return false;
    }
    else
    {
//    old version...
//        for (int i = 0; i < currentSize; i++)
//        {
//            if (!strcmp(theDic[i].tellWord(), oldWord))
//            {
//                for (int j = i; j < currentSize; j++)
//                {
//                    theDic[j] = theDic[j + 1];
//                };
//            currentSize--;
//            return true;
//            };
//        };
//        return false;
        //wDPair delP = ;
        wDPair *delPair = findWord(oldWord);
        wDPair *y, *x;
        if (delPair == NULL)
        {
            return false;
        }
        else
        {
            if ((delPair->left == nil) || (delPair->right == nil))
            {
                y = delPair;
            }
            else
            {
                y = nextPair(delPair);
                if (y = NULL) return false;
            };
            if (delPair->left != nil)
            {
                x = y->left;
            }
            else
            {
                x = y->right;
            };
            if (x != nil)
            {
                x->p = y->p;
            };
            if (y->p == nil)
            {
                root = x;
            }
            else if (y == y->p->left)
            {
                y->p->left = x;
            }
            else
            {
                y->p->right = x;
            };
            if (y != delPair)
            {
                strcpy(delPair->theWord, y->theWord);
                strcpy(delPair->theDef, y->theDef);
            };
            return true;
        };
    };
};

bool dict::empty(void) const
{
    return (currentSize == 0);
};

bool dict::full(void) const
{
    return (currentSize == VSIZE);
};

int dict::tellSize(void) const
{
    return currentSize;
};

const wDPair* dict::tellFirst(void)
{
    if (empty())
    {
        return NULL;
    }
    else
    {
//    old version...
//        currentLoc = 0;
//        return & theDic[currentLoc];
        wDPair *x = root;
        while (x->right != nil)
        {
            x = x->right;
        };
        currentLoc = x;
        return x;
    };
};

const wDPair* dict::tellNext(void)
{
    wDPair *x = currentLoc;
    if (x->right != nil)
    {
        x = x->right;
        while (x->left != nil)
        {
            x = x->left;
        };
        currentLoc = x;
        return currentLoc;
    }
    else
    {
        while (x != x->p->left)
        {
           x = x->p;
           if (x == root)
           {
               return NULL;
           };
        };
        currentLoc = x;
        return currentLoc;
    };
//    old version...
//    else if (x == x->p->left)
//    {
//        currentLoc = x->p;
//        return currentLoc;
//    }
//    else
//    {
//        while (x != x->p->left)
//    if(currentLoc == (currentSize - 1))
//    {
//        return NULL;
//    }
//    else
//    {
//        return & theDic[++currentLoc];
//    };
};

bool dict::diskSave(const char* fileName)
{
    ofstream saveStream(fileName);
    if (!saveStream)
    {
        return false;
    }
    else
    {
        saveStream << myDic->tellFirst() << '\n';
        for (int i = 1; i < currentSize; i++)
        {
            saveStream << myDic->tellNext() << '\n';
        };
        saveStream.close();
        return true;
    };
};

bool dict::diskRecover(const char* fileName)
{
    char wordBuf[81], defBuf[81];
    ifstream dictStream(fileName);
    if (!dictStream)
    {
        return false;
    }
    else
    {
        wDPair * thePair;
        currentSize = 0;
        while (dictStream.getline(wordBuf, 80) &&
                dictStream.getline(defBuf, 80) && (currentSize < VSIZE))
        {
            thePair = new wDPair(wordBuf, defBuf);
            insertPair(thePair);
        };
        dictStream.close();
        return true;
    };
};

wDPair * dict::nextPair(wDPair* aPair)
{
    wDPair *x = aPair;
    if (x->right != nil)
    {
        x = x->right;
        while (x->left != nil)
        {
            x = x->left;
        };
        return x;
    }
    else
    {
        while (x != x->p->left)
        {
           x = x->p;
           if (x == root)
           {
               return NULL;
           };
        };
        return x;
    };
};

//Details for Class: Menu
menu::menu(int howMany, char** theMenu, void(** theFunctions)(void))
{
    numItems = howMany;
    menuItems = theMenu;
    theChoices = theFunctions;
    theChoice = '0';
};

menu::~menu()
{
    //nothing created, nothing to destroy...
};

void menu::display() const
{
    cout << endl << endl << setw(45) << "MAIN MENU" << endl << endl;
    for (int i = 0; i <numItems; i++)
    {
        cout << setw(20) << (i+1) << ". " << menuItems[i] << endl;
    };
    cout << endl;
};

char menu::getChoice(void)
{
    const char maxChoice = ('0' + numItems);        //numItems cast as char...
    char aChoice;                                   //input is char to minimize error
    cout << "Enter the NUMBER of your choice: ";
    cin >> aChoice;
    while((aChoice < '1')||(aChoice > maxChoice))
    {
        cout << "Please select a number between 1 and " << maxChoice << ": ";
        cin >> aChoice;
    };
    theChoice = aChoice;
    return aChoice;
};

bool menu::done(void) const
{
    return(theChoice == ('0' + numItems));
};

void menu::doChoice(char & thisChoice) const
{
    (*theChoices[thisChoice - '0' - 1])();
};

void menu::typicalQuit(void)
{
    char yesNo;
    cout << endl << endl << setw(45) << "Quitting..." << endl << endl;
    cout << "Are you sure...? (Y/N)";
    cin >> yesNo;
    yesNo = toupper(yesNo);
    while((yesNo != 'Y')&&(yesNo != 'N'))
    {
        cout << "Please enter Y or N: ";
        cin >> yesNo;
        yesNo = toupper(yesNo);
    };
    if (yesNo == 'N')
    {
        theChoice = '0';
        cout << "Press any key to continue: ";
    }
    else
    {
        cout << endl << endl << "Goodbye. Press any key to exit: ";
    };
    cin.ignore();
    cin.get();
    system("cls");
};

void doAdd(void)
{
    system("cls");
    cout << endl << endl << setw(45) << "ADDING A WORD..." << endl << endl;
    if(myDic->full())
    {
        cout << "Dictionary is full. Please delete an entry to make room." << endl;
    }
    else
    {
        char aWord[81], aDef[81];
        cin.ignore();
        cout << "Please enter the word: ";
        cin.getline(aWord, 80);
        const wDPair * testPair = myDic->findWord(aWord);
        if (testPair != NULL)
        {
            cout << "This word is already in the dictionary: " << testPair->tellWord()
                << " - " << testPair->tellDef() << endl;
        }
        else
        {
            cout << "Please enter the definition: ";
            cin.getline(aDef, 80);
            wDPair * thePair = new wDPair(aWord, aDef);
            myDic->insertPair(thePair);
            cout << endl << setw(45) << "DONE..." << endl;
        };
    };
    cout << endl << "Press any key to continue...";
    cin.get();
    system("cls");
};


void doFind(void)
{
    system("cls");
    cout << endl << endl << setw(45) << "FINDING A WORD..." << endl << endl;
    if(myDic->empty())
    {
        cout << "Dictionary is empty. Nothing to see here..." << endl;
    }
    else
    {
        char theEntry[81];
        cin.ignore();
        cout << "Please enter the word: ";
        cin.getline(theEntry, 80);
        const wDPair *thePair = myDic->findWord(theEntry);
        if (thePair == NULL)
        {
            cout << theEntry << " was not found." << endl;
        }
        else
        {
            cout << "The definition of " << theEntry << " is "
                << thePair->tellDef() << endl;
        };
    };
    cout << endl << "Press any key to continue...";
    cin.ignore();
    cin.get();
    system("cls");
};

void doList(void)
{
    system("cls");
    cout << endl << endl << setw(45) << "LISTING THE DICTIONARY..." << endl << endl;
    if(myDic->empty())
    {
        cout << "Dictionary is empty. Nothing to see here..." << endl;
    }
    else
    {
        const wDPair * thePair = myDic->tellFirst();
        int i = 1;
        while (thePair != NULL)
        {
            cout << setw(20) << i++ << ". " << setw(15)
                << thePair->tellWord() << " - " << thePair->tellDef() << endl;
            thePair = myDic->tellNext();
        };
    };
    cout << endl << "Press any key to continue...";
    cin.ignore();
    cin.get();
    system("cls");
};

void doDelete(void)
{
    system("cls");
    cout << endl << endl << setw(45) << "DELETING AN ENTRY..." << endl << endl;
    if(myDic->empty())
    {
        cout << "Dictionary is empty. Nothing to delete..." << endl;
    }
    else
    {
        char theEntry[81];
        cin.ignore();
        cout << "Enter the word to be deleted: " << flush;
        cin.getline(theEntry, 80);
        const wDPair *testPair = myDic->findWord(theEntry);
        if (testPair == NULL)
        {
            cout << theEntry << " was not found." << endl;
        }
        else
        {
            myDic->deletePair(theEntry);
            cout << theEntry << " has been deleted." << endl;
        };
    };
    cout << endl << "Press any key to continue...";
    cin.ignore();
    cin.get();
    system("cls");
};

void doQuit(void)
{
    system("cls");
    theMenu->typicalQuit();
    system("cls");
};

void initialize1(void)
{
    myDic->diskRecover(OURDATAFILE);
};

void terminate1(void)
{
    myDic->diskSave(OURDATAFILE);
};


