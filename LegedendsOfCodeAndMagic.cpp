#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include<map>
using namespace std;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
//bool operator == (const pair<int ,int > & firstPair, const pair<int, int > & secondPair) {
//	return firstPair.first == secondPair.first && firstPair.second == secondPair.second;
//}

namespace GameConstants {
	const int maxHandCardsAmount = 8;
	const int maxBoardCardsAmount = 6;
}
enum CardNumber {
	FIRST = 0,
	SECOND,
	THIRD
};
class Action {
public:
	virtual void perform(ostream& o) const = 0 ;
	virtual ~Action() {}
};
struct ActionPass : public Action {
public:
	virtual void perform(ostream& o)const override {
		cout << "PASS" << ";";
	}
};

struct ActionAttack : public Action {
	int idAttacker;
	int idEnemy;
public:
	ActionAttack(int instanceIdAttacker, int instanceIdEnemy = -1) :
		idAttacker(instanceIdAttacker), idEnemy(instanceIdEnemy) {}
	virtual void perform(ostream& o)const override {
		cout << "ATTACK " << idAttacker << " " << idEnemy << ";";
	}
};

struct ActionSummon : public Action {
	int creatureID;
public:
	ActionSummon(int pCreatureID) : creatureID(pCreatureID) {}
	virtual void perform(ostream& o)const override {
		cout << "SUMMON " << creatureID << ";";
	}
};

struct ActionPick : public Action {
	CardNumber cardNum;
public:
	ActionPick(CardNumber cardNum) : cardNum(cardNum) {}
	virtual void perform(ostream& o)const override {
		cout << "PICK " << cardNum;
	}
};

struct ActionUse: public Action {
	int spellId;
	int enemyId;
public:
	ActionUse(int spellInstaceId,int enemyInstanceId= -1) : spellId(spellInstaceId), enemyId(enemyInstanceId) {}
	virtual void perform(ostream& o)const override {
		cout << "USE " << spellId << " " << enemyId << ";";
	}
};




struct Card {
	enum class Location {
		OpponentsBoard = -1,
		InHand = 0,
		OnBoard = 1
	};
	enum class Type {
		Creature = 0,
		GreenItem,
		RedItem,
		BlueItem
	};
	enum Abilities {
		Breakthrough = 0b00000001,
		Charge =	   0b00000010,
		Drain =        0b00000100,
		Guard =        0b00001000,
		Lethal =       0b00010000,
		Ward =         0b00100000
	};
	Card(istream& i);
	int cardId;
	int instanceId;
	Location location;
	Type type;
	int cost;
	int attack;
	int defense;
	unsigned int abilities = 0;
	int myHealthChange;
	int opponentHealthChange;
	int cardDraw;
	bool canAttack = false;
	string ToString() const;

};

bool operator == (const Card& first, const Card& second) {
	return first.instanceId == second.instanceId;
}

enum class GameState {
	DRAFT,
	BATTLE
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
	vector<Card> board;
public:
	Opponent(istream& i);
	inline void AddCard(Card c) { board.push_back(c); }
	inline const vector<Card>& GetBoard() const { return board; };
};

class Battler {
private:
	vector<Card>& myHand;
	vector<Card>& myBoard;
	vector<Card>& enemyBoard;
	vector<Action* > actions;
	int RemoveEnemyCard(Card& enemyCard);
	int RemoveMyCard(Card& myCard);

	int GoodEnemyTrade(Card& enemyCard);
	int GoodMyTrade(Card& myCard);
	int TradeSomehow(Card& enemyCard);

	Card* FindCardWithSmallestAttack();
	int BreakEnemyBuble(Card& targetCard);
	bool CanAttackCard(Card& enemyCard);
	bool EnemyHasTaunts() const;
	int AttackFace(Card& myCard);
public:
	Battler(vector<Card>& myHand, vector<Card>& mBoard, vector<Card>& oppBoard);
	void Battle();

};

class Me : public Player {
	vector<Card> hand;
	vector<Card> board;
	vector<Card> opponentsBoardCopy;
	Battler m_Battler;

	int cardsAmount = -1;
	vector<Action* > actions;
	void DraftThink();
	void BattleThink();
	GameState state = GameState::DRAFT;
	void PlayCards();
public:
	Me(istream& i);
	void GetCards(istream& i,Opponent & op);
	string ToString() const;
	void MakeTurn();
	void SetState(GameState state);
};



int main()
{
	int kek = 0;
	// game loop
	while (1) {
		Me m(cin);
		Opponent p(cin);
		m.GetCards(cin,p);
		m.SetState(kek < 30 ? GameState::DRAFT : GameState::BATTLE);
		m.MakeTurn();
		kek++;
		
	}
}

Card::Card(istream& i)
{
	int nlocation;
	int iCardType;
	string abs;
	cin >> cardId >> instanceId >> nlocation >> iCardType >>
		cost >> attack >> defense >> 
		abs >> myHealthChange >> opponentHealthChange
		>> cardDraw; cin.ignore();

	location = static_cast<Card::Location>(nlocation);
	type = static_cast<Card::Type>(iCardType);

	if (abs.find('B') != string::npos)
		abilities |= Card::Abilities::Breakthrough;
	if (abs.find('C') != string::npos)
		abilities |= Card::Abilities::Charge;
	if (abs.find('D') != string::npos)
		abilities |= Card::Abilities::Drain;
	if (abs.find('G') != string::npos)
		abilities |= Card::Abilities::Guard;
	if (abs.find('L') != string::npos)
		abilities |= Card::Abilities::Lethal;
	if (abs.find('W') != string::npos)
		abilities |= Card::Abilities::Ward;

	canAttack = location == Location::OnBoard;


}

string Card::ToString() const
{
	stringstream s;
	s << "Card location " << (int)location << '\n';
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

Me::Me(istream& i) : Player(cin), m_Battler(hand,board,opponentsBoardCopy)
{}

void Me::GetCards(istream& i,Opponent & p)
{
	
	cin >> cardsAmount; cin.ignore();
	cerr << "Getting cards amount " << cardsAmount << '\n';
	for (int j = 0; j < cardsAmount; j++) {
		Card card{ i };
		cerr << card.ToString() << '\n';
		if (card.location == Card::Location::InHand)
			hand.push_back(card);
		else if (card.location == Card::Location::OnBoard) {
			board.push_back(card);			
		}
		else if (card.location == Card::Location::OpponentsBoard) {
			p.AddCard(card);
		}
		else {
			cerr << "NO CARD TYPE WTF\N";
		}
	}

	opponentsBoardCopy = p.GetBoard();
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


void Me::MakeTurn()
{
	if (state == GameState::DRAFT)
		DraftThink();
	else BattleThink();
	if (actions.empty()) {
		auto* a = new ActionPass();
		a->perform(cout);
		delete a;
	}
	for (const auto& action : actions) {
		action->perform(cout); 
		delete action;
	}
	cout << "\n";

}

void Me::SetState(GameState state)
{
	this->state = state;
}

void Me::DraftThink()
{
	if (!hand.empty())
		cerr << "In hand\n";
	else cerr << "not in hand\n";
	int i = 0;
	while (i < 3) {
		if (hand[i].type != Card::Type::RedItem && hand[i].type != Card::Type::BlueItem) {
			actions.push_back(new ActionPick((CardNumber)i));
			return;
		}
		i++;
	}
	actions.push_back(new ActionPick(FIRST));
}

void Me::BattleThink()
{
	PlayCards();
	m_Battler.Battle();
}

void Me::PlayCards() {

	vector<Card> playableCards;
	copy_if(hand.begin(), hand.end(), back_inserter(playableCards), [&](Card card) {
		if (card.cost == mana) {
			if (card.type == Card::Type::GreenItem ) {
				if (!board.empty()) {
					playableCards.push_back(card);
					mana -= card.cost;
					return true;
				}
				
			}
			else if (card.type == Card::Type::Creature) {
				playableCards.push_back(card);
				mana -= card.cost;
				return true;
			}			
		}
		return false;
		});

	if (playableCards.empty()) {
		copy_if(hand.begin(), hand.end(), back_inserter(playableCards), [&](Card card) {
			if (card.cost <= mana) {
				if (card.type == Card::Type::GreenItem) {
					if (!board.empty()) {
						playableCards.push_back(card);
						mana -= card.cost;
						return true;
					}

				}
				else if (card.type == Card::Type::Creature) {
					playableCards.push_back(card);
					mana -= card.cost;
					return true;
				}
			}
			return false;
			});
	}
	for_each(playableCards.begin(), playableCards.end(), [&](Card card) {
		if (card.type == Card::Type::Creature) {
			actions.push_back(new ActionSummon(card.instanceId));
		}
		else if (card.type == Card::Type::GreenItem) {
			cerr << "Adding USE action\n";
			actions.push_back(new ActionUse(card.instanceId, board.front().instanceId));
		}
		});

}

Battler::Battler(vector<Card>& myHand, vector<Card>& mBoard, vector<Card>& oppBoard) :
	myHand(myHand),myBoard(mBoard),enemyBoard(oppBoard)
{
}

void Battler::Battle()
{

	if (EnemyHasTaunts()) {
		for (auto& card : enemyBoard) {
			if (card.abilities & Card::Abilities::Guard) {
				GoodEnemyTrade(card);
			}
		}
		if (EnemyHasTaunts()) {
			for (auto& card : enemyBoard) {
				if (card.abilities & Card::Abilities::Guard) {
					TradeSomehow(card);
				}
			}
		}
	}

	if (!EnemyHasTaunts()) {
		for (auto& card : enemyBoard) {
			GoodEnemyTrade(card);
		}
		for (auto& card : myBoard) {
			AttackFace(card);
		}
	}
	if (actions.empty()) {
		auto t = new ActionPass();
		t->perform(cout);
		delete t;
		return;
	}
	for (auto* action : actions) {
		action->perform(cout);
		delete action;
	}
}


int Battler::RemoveEnemyCard(Card& enemyCard)
{
	enemyBoard.erase(remove(enemyBoard.begin(), enemyBoard.end(), enemyCard));
	return 1;
}

int Battler::RemoveMyCard(Card& myCard)
{
	myBoard.erase(remove(myBoard.begin(), myBoard.end(), myCard));
	return 1;
}

int Battler::GoodEnemyTrade(Card& enemyCard)
{
	if(enemyCard.abilities & Card::Abilities::Ward)
		BreakEnemyBuble(enemyCard);

	vector<int> cardsAttackDiff;
	int min = 0;
	for (int i = 0; i < myBoard.size();++i) {
		if (enemyCard.defense - myBoard[i].attack <= min) 
			cardsAttackDiff.push_back(i);
		
	}
	Card* bestCard = nullptr;
	for (int i = 0; i < cardsAttackDiff.size(); ++i) {
		auto& currCard = myBoard[cardsAttackDiff[i]];
		if (enemyCard.attack < currCard.defense) {
			bestCard = &currCard;
		}
	}
	if (bestCard) {
		RemoveEnemyCard(enemyCard);
		actions.push_back(new ActionAttack(bestCard->instanceId, enemyCard.instanceId));
	}
	return bestCard ? 1 : 0;
}

int Battler::GoodMyTrade(Card& myCard)
{
	return 0;
}

int Battler::TradeSomehow(Card& enemyCard)
{
	if (enemyCard.abilities & Card::Abilities::Ward)
		BreakEnemyBuble(enemyCard);
	vector<int> attackedIndexes;
	for (auto& c : myBoard) {
		actions.push_back(new ActionAttack(c.instanceId, enemyCard.instanceId));
		enemyCard.defense -= c.attack;
		c.defense -= enemyCard.attack;
		if (c.defense <= 0) {
			RemoveMyCard(c);
		}
		if (enemyCard.defense <= 0) {
			RemoveEnemyCard(enemyCard);
		}
	}
	return 0;
}

Card * Battler::FindCardWithSmallestAttack()
{
	if (myBoard.empty()) {
		return nullptr;
	}
	Card* res = nullptr;
	int smallestAttack = 100;
	int smallestHealth = 100;
	for (auto& card : myBoard) {
		if (card.attack < smallestAttack && card.defense < smallestHealth) {
			res = &card;
			smallestAttack = card.attack;
			smallestHealth = card.defense;
		}	
	}
	return res;
}

int Battler::BreakEnemyBuble(Card& targetCard)
{
	Card* wekestCard = FindCardWithSmallestAttack();
	if (wekestCard && CanAttackCard(targetCard)) {
		actions.push_back(new ActionAttack(wekestCard->instanceId, targetCard.instanceId));
		if (wekestCard->defense <= targetCard.attack)
			RemoveMyCard(*wekestCard);
		return 1;
	}
	return 0;
}

bool Battler::CanAttackCard(Card& enemyCard)
{
	if (enemyCard.abilities & Card::Abilities::Guard)
		return true;

	return EnemyHasTaunts();
}

bool Battler::EnemyHasTaunts() const
{
	for (const auto& card : enemyBoard) 
		if (card.abilities & Card::Abilities::Guard)
			return true;
	
	return false;
}

int Battler::AttackFace(Card& myCard) 
{
	if(myCard.canAttack) 
		actions.push_back(new ActionAttack(myCard.instanceId));
	myCard.canAttack = false;
	return !myCard.canAttack;
}
