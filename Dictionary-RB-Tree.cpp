/*
Dictionary Project using Red-Black tree
UMES CSDP 601
D. Raizen
*/
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctype.h>

using namespace std;
const int VSIZE = 100;

const char OURDATAFILE[] = "c:\\Space\\junk.dat";
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
        char* theWord;
        char* theDef;
        char color;         //to implement RBTree
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
        const wDPair* tellRoot() const;
        bool diskSave(const char*);     //file name - false on fail
        bool diskRecover(const char*);

    protected:
        //wDPair theDic[VSIZE];           //10-10 changing this to a BS Tree...
        void leftRot(wDPair*);
        void rightRot(wDPair*);
        bool insertFix(wDPair*);
        bool deleteFix(wDPair*);
        wDPair * nextPair(wDPair*);     //Non-constant next function used in the deletePair function.
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
//    cout << "New wDPair constructed..."<<endl;
    if (newWord != NULL)
    {
        theWord = new char[strlen(newWord)+1];
        strcpy(theWord, newWord);
    }
    else
    {
        theWord = NULL;
    };
    if (newDef != NULL)
    {
        theDef = new char[strlen(newDef)+1];
        strcpy(theDef, newDef);
    }
    else
    {
        theDef = NULL;
    };
    this->p = this->left = this->right = NULL;
    this->color = 'R';
};


wDPair::wDPair(const wDPair & otherPair)
{
    if (otherPair.theWord == NULL)
    {
        theWord = NULL;
    }
    else
    {
        delete[] theWord;
        theWord = new char[strlen(otherPair.theWord)+1];
        strcpy(theWord, otherPair.theWord);
    }
    if (otherPair.theDef == NULL)
    {
        theDef = NULL;
    }
    else
    {
        delete[] theDef;
        theDef = new char[strlen(otherPair.theDef)+1];
        strcpy(theDef, otherPair.theDef);
    }
};

wDPair::~wDPair()
{
    if (theWord != NULL)    //Delete pair...
    {
        delete[] theWord;
    }
    if (theDef != NULL)
    {
        delete[] theDef;
    }
};

wDPair & wDPair::operator= (const wDPair & otherPair)
{
    if (this != & otherPair)
    {
        if (otherPair.theWord == NULL)
        {
            theWord = NULL;
        }
        else
        {
            delete[] theWord;
            theWord = new char[strlen(otherPair.theWord)+1];
            strcpy(theWord, otherPair.theWord);
        }
        if (otherPair.theDef == NULL)
        {
            theDef = NULL;
        }
        else
        {
            delete[] theDef;
            theDef = new char[strlen(otherPair.theDef)+1];
            strcpy(theDef, otherPair.theDef);
        }
        this->p = otherPair.p;                   //Must copy a node's pointers as well as it's data...
        this->left = otherPair.left;
        this->right = otherPair.right;
        this->color = otherPair.color;
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
    delete[] theWord;
    theWord = new char[strlen(newWord)+1];
    strcpy(theWord, newWord);
};

void wDPair::changeDef(const char * newDef)
{
    delete[] theDef;
    theDef = new char[strlen(newDef)+1];
    strcpy(theDef, newDef);
};



//Details for the Class dict...
//Many changes here to implement BSTree
dict::dict(char* fileName)
{
//    cout << "New dict..."<<endl;
    currentSize = 0;
    if(fileName != NULL)
    {
//        cout<<"fileName != NULL"<<endl;
        diskRecover(fileName);
    }
    else
    {
//        cout<<"fileName == NULL"<<endl;
        nil = new wDPair();
        nil->theWord = "";
        nil->color = 'B';
        root = NULL;            //set the root initially to NULL
        currentLoc = root;
    };
//    cout<<"Nil: "<<nil->theWord<<endl;

};

dict::~dict()
{
    //nothing to do... yet?
};

wDPair * dict::findWord(const char * searchWord)
{
    wDPair *x = root;
//    cout << "*x = root..."<<endl;
    if (x == NULL) return NULL;
//    cout<<"searchWord:"<<searchWord<<", x->theWord:"<<x->theWord<<endl;
    while ((x != nil) && (strcmp(searchWord, x->theWord) != 0))
    {
//        cout<<"(x != nil), and ";
//        cout << "(searchWord != x->theWord):"<<searchWord<<", "<<x->theWord<<endl;
        if (strcmp(searchWord, x->theWord) < 0)
        {
//            cout << "searchWord < x->theWord"<<endl;
//            cout << "(searchWord != x->theWord):"<<searchWord<<", "<<x->theWord<<endl;
            x = x->left;
        }
        else
        {
//            cout << "searchWord > x->theWord"<<endl;
//            cout << "(searchWord != x->theWord):"<<searchWord<<", "<<x->theWord<<endl;
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
//    cout<<"insertPair..."<<endl;
    if(currentSize == VSIZE)
    {
        return false;
    }
    else
    {
        wDPair *y = NULL;
//        cout<<"wDPair *y = NULL;";
        wDPair *x = root;
//        cout<<"wDPair *x = root;"<<endl;
        if (x == NULL)
        {
//            cout<<"currentSize == 0"<<endl;
            root = newPair;
//            cout<<"root=newPair"<<endl;
            newPair->p = newPair->left = newPair->right = nil;
            newPair->color = 'B';
//            cout<<"newPair pointers fixed..."<<endl;
            currentSize++;
//            cout<<"currentSise: "<<currentSize<<endl;
//            cout<<"root word, par, left, right, color: "<<root->theWord<<" "
//                <<((root->p==nil)?"NULL":"Not NULL")<<" "<<((root->left==nil)?"NULL":"Not NULL")<<" "
//                <<((root->right==nil)?"NULL ":"Not NULL ")<<root->color<<endl;

            return true;
        }
        else
        {
//            cout<<"root!=nil"<<endl;
            while (x != nil)
            {
//                cout<<"x != nil"<<endl;
                y = x;
                if (strcmp(newPair->theWord, x->theWord) < 0)
                {
                    x = x->left;
//                    cout<<"x = x->left"<<endl;
                }
                else
                {
                    x = x->right;
//                    cout<<"x = x->right"<<endl;
                };
            };
//            cout<<"x == nil"<<endl;
            newPair->p = y;
            newPair->left = newPair->right = nil;
            if (strcmp(newPair->theWord,y->theWord) < 0)
            {
                y->left = newPair;
            }
            else
            {
                y->right = newPair;
            };
//            cout<<"root word, par, left, right, color: "<<root->theWord<<" "
//                <<((root->p==nil)?"NULL":"Not NULL")<<" "<<((root->left==nil)?"NULL":"Not NULL")<<" "
//                <<((root->right==nil)?"NULL ":"Not NULL ")<<root->color<<endl;
//            cout<<"newWord, par, left, right:"<<newPair->theWord<<" "
//                <<newPair->p->theWord<<" "<<((newPair->left==nil)?"NULL":"Not NULL")<<" "
//                <<((newPair->right==nil)?"NULL":"Not NULL")<<endl;
            currentSize++;
//            cout<<"currentSize:"<<currentSize<<endl;
            newPair->color = 'R';
            insertFix(newPair);
            return true;
        };
    };
};

void dict::leftRot(wDPair *rotPair)
{
    wDPair *x = rotPair;
    wDPair *y = x->right;
    x->right = y->left;
    y->left->p = x;
    y->p = x->p;
    if (x->p == nil)
    {
        root = y;
    }
    else if (x == x->p->left)
    {
        x->p->left = y;
    }
    else
    {
        x->p->right = y;
    };
    y->left = x;
    x->p = y;
};

void dict::rightRot(wDPair *rotPair)
{
    wDPair *x = rotPair;
    wDPair *y = x->left;
    x->left = y->right;
    y->right->p = x;
    y->p = x->p;
    if (x->p == nil)
    {
        root = y;
    }
    else if (x == x->p->right)
    {
        x->p->right = y;
    }
    else
    {
        x->p->left = y;
    };
    y->right = x;
    x->p = y;
    return;
};

bool dict::insertFix(wDPair *newPair)
{
    wDPair *z = newPair;
    while (z->p->color == 'R')
    {
        if (z->p == z->p->p->left)
        {
            wDPair *y = z->p->p->right;
            if (y->color == 'R')
            {
                z->p->color = 'B';
                y->color = 'B';
                z->p->p->color = 'R';
                z = z->p->p;
            }
            else
            {
                if (z == z->p->right)
                {
                    z = z->p;
                    leftRot(z);
                };
                z->p->color = 'B';
                z->p->p->color = 'R';
                rightRot(z->p->p);
            };
        }
        else
        {
            wDPair *y = z->p->p->left;
            if (y->color == 'R')
            {
                z->p->color = 'B';
                y->color = 'B';
                z->p->p->color = 'R';
                z = z->p->p;
            }
            else
            {
                if (z == z->p->left)
                {
                    z = z->p;
                    rightRot(z);
                };
                z->p->color = 'B';
                z->p->p->color = 'R';
                leftRot(z->p->p);
            };
        };

    };
    root->color = 'B';
    return true;
};

bool dict::deleteFix(wDPair *newPair)
{
//    cout <<"deleteFix..."<<endl;
    wDPair *x = newPair;
    wDPair *w;
    while ((x != root) && (x->color == 'B'))
    {
      if (x == x->p->left)
      {
          w = x->p->right;
          if (w->color == 'R')
          {
              w->color = 'B';
              x->p->color = 'R';
              leftRot(x->p);
              w = x->p->right;
          };
          if ((w->left->color == 'B') && (w->right->color == 'B'))
          {
              w->color = 'R';
              x = x->p;
          }
          else
          {
              if (w->right->color == 'B')
              {
                  w->left->color = 'B';
                  w->color = 'R';
                  rightRot(w);
                  w = x->p->right;
              }
              w->color = x->p->color;
              x->p->color = 'B';
              w->right->color = 'B';
              leftRot(x->p);
              x = root;
          };
      }
      else
      {
          w = x->p->left;
          if (w->color == 'R')
          {
              w->color = 'B';
              x->p->color = 'R';
              rightRot(x->p);
              w = x->p->left;
          };
          if ((w->left->color == 'B') && (w->right->color == 'B'))
          {
              w->color = 'R';
              x = x->p;
          }
          else
          {
              if (w->left->color == 'B')
              {
                  w->right->color = 'B';
                  w->color = 'R';
                  leftRot(w);
                  w = x->p->left;
              }
              w->color = x->p->color;
              x->p->color = 'B';
              w->left->color = 'B';
              rightRot(x->p);
              x = root;
          };
      };
    };
    x->color = 'B';
    return true;
};

bool dict::deletePair(const char* oldWord)
{
//    cout<<"deletePair..."<<endl;
    if (currentSize == 0)
    {
        return false;
    }
    else
    {
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
//                cout<<"y: "<<y->theWord<<endl;
            };
            if (y->left != nil)
            {
                x = y->left;
            }
            else
            {
                x = y->right;
            };
            x->p = y->p;
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
            if (y->color == 'B')
            {
                deleteFix(x);
            };
            currentSize--;
            delete y;
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

const wDPair * dict::tellRoot() const
{
    return root;
};

int dict::tellSize(void) const
{
    return currentSize;
};

const wDPair* dict::tellFirst(void)
{
//    cout<<"tellFirst..."<<endl;
    if (empty())
    {
        return NULL;
    }
    else
    {
        wDPair *x = root;
//        cout<<"x = root:"<<root->theWord<<endl;
        while (x->left != nil)
        {
//            cout<<x->theWord<<endl;
            x = x->left;
        };
        currentLoc = x;
//        cout<<"currentLoc->theWord, color: "<<currentLoc->theWord<<", "<<currentLoc->color<<endl;
        return x;
    };
};

const wDPair* dict::tellNext(void)
{
//    cout<<"tellNext..."<<endl;
    currentLoc = nextPair(currentLoc);
    return currentLoc;
};

bool dict::diskSave(const char* fileName)
{
//    cout<<"diskSave may be implemented..."<<endl;
//    return true;
    ofstream saveStream(fileName);
    if (!saveStream)
    {
//        cout <<"No saveStream..."<<endl;
        return false;
    }
    else
    {
        const wDPair* toDo [myDic->tellSize()] ;
        const wDPair *savePair = myDic->tellRoot();
        toDo[0] = savePair;
        int  head = 0;
        int tail = 0;
        for (int i = 0; i < myDic->tellSize(); i++)
        {
            savePair = toDo[head];
            saveStream << toDo[head]->tellWord() << '\n' << toDo[head]->tellDef()
                << '\n'<< ((toDo[head]->left != nil)?'1':'0') << '\n' << ((toDo[head]->right!=nil)?'1':'0') <<'\n';
            if (toDo[head]->left!= nil)
            {
                tail++;
                toDo[tail] = savePair->left;
            };
            if (savePair->right!=nil)
            {
                tail++;
                toDo[tail] = savePair->right;
            };
            head++;
        };
        saveStream.close();
        return true;
    };
};

bool dict::diskRecover(const char* fileName)
{
//    cout<<"diskRecover implemented...?"<<endl;
    char wordBuf[81], defBuf[81], leftBuf[81], rightBuf[81];
    ifstream dictStream(fileName);
    if (!dictStream)
    {
//        cout<<"no stream..."<<endl;
        return false;
    }
    else
    {

        wDPair * thePair;
        currentSize = 0;
        dictStream.getline(wordBuf, 80);
        dictStream.getline(defBuf, 80);
        dictStream.getline(leftBuf, 80);
        dictStream.getline(rightBuf, 80);
//        cout << "wordBuf: " << wordBuf << endl;
        if (strcmp(wordBuf, "") == 0) return true;
        thePair = new wDPair(wordBuf, defBuf);
        myDic->insertPair(thePair);

        while (dictStream.getline(wordBuf, 80) &&
                dictStream.getline(defBuf, 80) && dictStream.getline(leftBuf, 80) &&
                dictStream.getline(rightBuf, 80) && (currentSize < VSIZE))
        {
            thePair = new wDPair(wordBuf, defBuf);
            insertPair(thePair);
        };
        dictStream.close();
//        cout<<"diskRecover done..."<<endl;
        return true;
    };
};

wDPair * dict::nextPair(wDPair* aPair)
{
//    cout<<"nextPair..."<<endl;
//    cout<<"currentLoc->theWord, aPair->theWord: "<<currentLoc->theWord
//            <<", "<<aPair->theWord<<endl;
    wDPair *x = aPair;
    if (x->right != nil)
    {
//        cout<<"x has a right child..."<<endl;
        x = x->right;
        while (x->left != nil)
        {
            x = x->left;
        };
//        cout<<"while loop has passed..."<<"x, x->color: "<<x->theWord<<", "<<x->color<<endl;
        return x;
    }
    else
    {
//        cout<<"x has no right child... x->p: "<<x->p->theWord<<endl;
        wDPair *y = x->p;
//        cout << "wDPair *y = x->p"<<endl;
        while ((y != nil) && (x == y->right))
        {
//           cout<<"y->theWord: "<<y->theWord<<endl;
           x = y;
           y = y->p;
        };
//        cout << "while loop passed..."<<"y, y->color: "<<y->theWord<<", "<<y->color<<endl;
        if (y == nil) return NULL;

        return y;
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
        cout << "Press the [ENTER] key to continue: ";
    }
    else
    {
        cout << endl << endl << "Goodbye. Press the [ENTER] key to exit: ";
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
            wDPair* thePair = new wDPair(aWord, aDef);
            myDic->insertPair(thePair);
            cout << endl << setw(45) << "DONE..." << endl;
        };
    };
    cout << endl << "Press the [ENTER] key to continue...";
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
    cout << endl << "Press the [ENTER] key to continue...";
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
}

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
    cout << endl << "Press the [ENTER] key to continue...";
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


