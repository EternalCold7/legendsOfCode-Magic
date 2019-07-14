#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
class Action {
public:
	virtual void perform(ostream& o) = 0;
	virtual ~Action() {}
};
struct ActionPass : public Action {
public:
	virtual void perform(ostream& o) override {
		cout << "PASS";
	}
};

struct ActionAttack : public Action {
	int idAttacker;
	int idEnemy;
public:
	ActionAttack(int idAttacker, int idEnemy) :
		idAttacker(idAttacker), idEnemy(idEnemy) {}
	virtual void perform(ostream& o) override {
		cout << "ATTACK " << idAttacker << " " << idEnemy;
	}
};

struct ActionSummon : public Action {
	int creatureID;
public:
	ActionSummon(int creatureID) : creatureID(creatureID) {}
	virtual void perform(ostream& o) override {
		cout << "SUMMON " << creatureID;
	}
};

struct ActionPick : public Action {
	int creatureID;
public:
	ActionPick(int creatureID) : creatureID(creatureID) {}
	virtual void perform(ostream& o) override {
		cout << "PICK " << creatureID;
	}
};


struct Card {
	enum class Location {
		OpponentsBoard = -1,
		InHand = 0,
		OnBoard = 1
	};

	Card(istream& i);
	int cardId;
	int instanceId;
	Location location;
	int cardType;
	int cost;
	int attack;
	int defense;
	string ab;
	int myHealthChange;
	int opponentHealthChange;
	int cardDraw;
	string ToString() const;
};

enum class GameState {

};




class Player {
protected:
	int health;
	int mana;
	int deckCardsAmount;
	int rune;
	int draw;
	
public:

	Player(istream& i);
};

class Opponent : public Player {
	int actionsCount;//last round actions count
	int handCardsCount;//last round actions count
	vector<string> actions;
public:
	Opponent(istream& i);
};

class Me : public Player {
	vector<Card> hand;
	int cardsAmount = -1;
public:
	Me(istream& i);
	void GetCards(istream& i);
	string ToString() const;
};



int main()
{
	int kek = 0;
	// game loop
	while (1) {
		Me m(cin);
		Opponent p(cin);
		m.GetCards(cin);
		
	//	cerr << m.ToString() << endl;
		if (kek < 30) {
			cout << "PICK 1"  << endl;
		}
		else {
			cout << "PASS" << endl;
		}
		kek++;
		
	}
}

Card::Card(istream& i)
{
	int nlocation;
	cin >> cardId >> instanceId >> nlocation >> cardType >>
		cost >> attack >> defense >> 
		ab >> myHealthChange >> opponentHealthChange
		>> cardDraw; cin.ignore();

	location = static_cast<Card::Location>(nlocation);
}

string Card::ToString() const
{
	stringstream s;
	s << "Card id " << cardId << '\n';
	s << "Card health " << defense << '\n';

	s << "Card attack " << attack << '\n';
	return s.str();
}

Player::Player(istream& i)
{
	cin >> health >> mana >> deckCardsAmount >> rune >> draw; cin.ignore();
}

Opponent::Opponent(istream& i) : Player(cin)
{
	cin >> handCardsCount >> actionsCount; cin.ignore();
	cerr << "Opponents Hand amount " << handCardsCount << '\n';
	for (int i = 0; i < actionsCount; i++) {
		string cardNumberAndAction;
		getline(cin, cardNumberAndAction);
		actions.push_back(cardNumberAndAction);
	}
}

Me::Me(istream& i) : Player(cin)
{}

void Me::GetCards(istream& i)
{
	
	cin >> cardsAmount; cin.ignore();
	cerr << "Getting cards amount " << cardsAmount << '\n';
	for (int j = 0; j < cardsAmount; j++) {
		Card a{ i };
		cerr << a.ToString() << '\n';
		hand.push_back(a);
	}
}

string Me::ToString() const
{

	stringstream s;
	s << "My health " << health << '\n'; 
	s << "My mana " << mana << '\n'; 
	s << "Cards count " << cardsAmount << '\n';

	for (const auto& card : hand)
		s << card.ToString();
	return s.str();
}
