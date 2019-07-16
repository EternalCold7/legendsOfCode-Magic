#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <sstream>
#include<map>
#include<set>
#include<numeric>
#include<cassert>
#include<chrono>
using namespace std;
using namespace std::chrono;

/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
 //bool operator == (const pair<int ,int > & firstPair, const pair<int, int > & secondPair) {
 //	return firstPair.first == secondPair.first && firstPair.second == secondPair.second;
 //}

enum {
	Me = 0,
	Opponent = 1
};
namespace GameConstants {
	const int maxHandCardsAmount = 8;
	const int maxBoardCardsAmount = 6;
	const int GoodCurve = 6;
}

enum CardNumber {
	FIRST = 0,
	SECOND,
	THIRD
};
class Action {
public:
	void Attack(int instanceIdAttacker, int instanceIdEnemy = -1) const {
		cout << "ATTACK " << instanceIdAttacker << " " << instanceIdEnemy << ";";
	}
	void Summon(int pCreatureID) const {
		cout << "SUMMON " << pCreatureID << ";";
	}
	void Pick(int cardNumber) const {
		cout << "PICK " << cardNumber;
	}
	void Use(int spellInstaceId, int enemyInstanceId = -1) const {
		cout << "USE " << spellInstaceId << " " << enemyInstanceId << ";";
	}
	void Pass() const {
		cout << "PASS" << ";";
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
		Charge = 0b00000010,
		Drain = 0b00000100,
		Guard = 0b00001000,
		Lethal = 0b00010000,
		Ward = 0b00100000
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

float calcCardValue(Card& c);


class Player {
protected:
	int health = -1;
	int mana = -1;
	int deckCardsAmount = -1;
	int rune = -1;
	int draw = -1;

public:
	int GetMana() const { return mana; }
	void SpendMana(int minus) { mana -= minus; }
	int GetHealth() const { return health; }
	Player(istream& i);
	Player() = default;
};

class Drafter {
	vector<Card>& hand;

public:
	Drafter(vector<Card>& h) : hand(h) {}
	void MakeChoice();
};

class State {
	Player players[2];
	GameState state;
	vector<Card> myHand;
	vector<Card> myBoard;
	vector<Card> enemyBoard;
	Drafter m_Drafter;



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
	void Battle();
	State(istream& i);
	void Think();
};

map<int, int> byCurve;
map<Card::Type, int> byType;
int main()
{
	while (1) {
		high_resolution_clock::time_point p1 = high_resolution_clock::now();
		State s(cin);
		s.Think();
		high_resolution_clock::time_point p2 = high_resolution_clock::now();
		cerr << "Turn time " << duration_cast<milliseconds>(p2 - p1).count() << '\n';
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




void State::Battle()
{
	cerr << "Enemy " << (EnemyHasTaunts() ? " has " : " have not ") << "taunts\n";
	if (EnemyHasTaunts()) {
		for (auto& card : enemyBoard) {
			if (card.abilities & Card::Abilities::Guard) {
				GoodEnemyTrade(card);
			}
		}

	}
	if (EnemyHasTaunts())
	{
		for (auto& card : enemyBoard) {
			if (card.abilities & Card::Abilities::Guard) {
				TradeSomehow(card);
			}
		}
	}



	for (auto& card : enemyBoard) {
		if (card.abilities & Card::Abilities::Ward) {
			BreakEnemyBuble(card);
		}
		GoodEnemyTrade(card);
	}
	for (auto& card : myBoard) {
		AttackFace(card);
	}
	Action a;
	a.Pass();	

}


int State::RemoveEnemyCard(Card& enemyCard)
{
	enemyBoard.erase(remove(enemyBoard.begin(), enemyBoard.end(), enemyCard));
	return 1;
}

int State::RemoveMyCard(Card& myCard)
{
	myBoard.erase(remove(myBoard.begin(), myBoard.end(), myCard));
	return 1;
}

int State::GoodEnemyTrade(Card& enemyCard)
{
	cerr << "In good enemy trade enemy card id " << enemyCard.instanceId << '\n';
	if (enemyCard.abilities & Card::Abilities::Ward)
		BreakEnemyBuble(enemyCard);

	vector<int> goodCardIndx;
	int min = 0;
	for (int i = 0; i < myBoard.size(); ++i) {
		if (myBoard[i].canAttack) {
			if (enemyCard.defense - myBoard[i].attack <= min)
				goodCardIndx.push_back(i);
		}



	}
	Card* bestCard = nullptr;

	for (int i = 0; i < goodCardIndx.size(); ++i) {
		auto& currCard = myBoard[goodCardIndx[i]];
		if (currCard.canAttack)
			if (enemyCard.attack < currCard.defense) {
				bestCard = &currCard;
			}
			else if ((enemyCard.defense - currCard.defense) >= 3) {
				bestCard = &currCard;
			}

	}

	Action a;
	if (bestCard) {

		a.Attack(bestCard->instanceId, enemyCard.instanceId);
		cerr << "Traded card " << enemyCard.instanceId << "By card " << bestCard->instanceId << '\n';
		bestCard->canAttack = false;
		RemoveEnemyCard(enemyCard);
	}
	return bestCard ? 1 : 0;
}

int State::GoodMyTrade(Card& myCard)
{
	return 0;
}

int State::TradeSomehow(Card& enemyCard)
{
	cerr << "in trade somehow " << enemyCard.instanceId << "\n";
	if (enemyCard.abilities & Card::Abilities::Ward)
		BreakEnemyBuble(enemyCard);
	vector<int> attackedIndexes;
	for (auto& c : myBoard) {
		if (!c.canAttack)
			continue;
		Action a;
		a.Attack(c.instanceId, enemyCard.instanceId);
		c.canAttack = false;
		enemyCard.defense -= c.attack;
		c.defense -= enemyCard.attack;
		if (c.defense <= 0) {
			RemoveMyCard(c);
		}
		if (enemyCard.defense <= 0) {
			RemoveEnemyCard(enemyCard);
			return 1;
		}
	}
	return 0;
}

Card* State::FindCardWithSmallestAttack()
{
	if (myBoard.empty()) {
		return nullptr;
	}
	Card* res = nullptr;
	int smallestAttack = 100;
	int smallestHealth = 100;
	for (auto& card : myBoard) {
		if (!card.canAttack)
			continue;
		if (card.attack < smallestAttack && card.defense < smallestHealth) {
			res = &card;
			smallestAttack = card.attack;
			smallestHealth = card.defense;
		}
	}
	return res;
}

int State::BreakEnemyBuble(Card& targetCard)
{
	Card* wekestCard = FindCardWithSmallestAttack();
	if (wekestCard->attack > 2)
		return 0;
	if (wekestCard && CanAttackCard(targetCard)) {
		Action a;
		a.Attack(wekestCard->instanceId, targetCard.instanceId);
		wekestCard->canAttack = false;
		if (wekestCard->defense <= targetCard.attack)
			RemoveMyCard(*wekestCard);
		return 1;
	}
	return 0;
}

bool State::CanAttackCard(Card& enemyCard)
{
	if (enemyCard.abilities & Card::Abilities::Guard)
		return true;

	return EnemyHasTaunts();
}

bool State::EnemyHasTaunts() const
{
	for (const auto& card : enemyBoard)
		if (card.abilities & Card::Abilities::Guard)
			return true;

	return false;
}

int State::AttackFace(Card& myCard)
{
	Action a;
	if (myCard.canAttack)
		a.Attack(myCard.instanceId);
	myCard.canAttack = false;
	return !myCard.canAttack;
}

void Drafter::MakeChoice()
{
	int cartToPlayIndex = 0;

	float bigger = 0.0;

	for (int j = 0; j < hand.size(); ++j) {
		float currValue = calcCardValue(hand[j]);
		cerr << "Card id " << hand[j].cardId << " value " << currValue << '\n';
		if (currValue > bigger) {
			bigger = currValue;
			cartToPlayIndex = j;
		}
	}
	Action a;
	a.Pick(cartToPlayIndex);

}

float calcCardValue(Card& c)
{
	//TODO MAKE IN CLASS; AND MAP WITH ABILITIES SCALE
	const static std::set<Card::Abilities> allAbilities = {
		Card::Abilities::Breakthrough,
		Card::Abilities::Charge,
		Card::Abilities::Drain,
		Card::Abilities::Guard,
		Card::Abilities::Lethal,
		Card::Abilities::Ward };

	float value = 0.f;
	if (c.cost == 0 || c.attack == 0)
		return value;
	float attackCoff = c.attack / c.cost;
	float defenseCoff = c.defense / c.cost;

	value += attackCoff;
	value += defenseCoff;
	float statsDiff = abs(c.attack - c.defense);
	if (statsDiff > 5 && c.cost < 8) {
		value -= 4.f;
	}
	if (statsDiff) {
		value += 1.5f / statsDiff;
	}
	else {
		value += 1.f;
	}
	for (const auto& ability : allAbilities) {
		if (c.abilities & ability)
			value += (0.2 * c.cost);
	}
	int curve = abs(GameConstants::GoodCurve - c.cost);
	if (curve) {
		value += 7.f / curve;
	}
	else {
		value += 6.5f;
	}
	if (c.cost < 3) {
		value -= 4.f;
	}
	value += c.cost * 0.2;
	return value;
}

State::State(istream& i) :  m_Drafter(myHand)
{
	players[Me] = Player(i);
	players[Opponent] = Player(i);

	vector<string> actions;
	int handCardsCount, actionsCount;
	cin >> handCardsCount >> actionsCount; cin.ignore();

	for (int i = 0; i < actionsCount; i++) {
		string cardNumberAndAction;
		getline(cin, cardNumberAndAction);
		actions.push_back(cardNumberAndAction);
	}
	int cardsAmount;
	cin >> cardsAmount; cin.ignore();

	for (int j = 0; j < cardsAmount; j++) {
		Card card{ i };

		if (card.location == Card::Location::InHand)
			myHand.push_back(card);
		else if (card.location == Card::Location::OnBoard) {
			myBoard.push_back(card);
		}
		else if (card.location == Card::Location::OpponentsBoard) {
			enemyBoard.push_back(card);
		}
		else {
			cerr << "NO CARD TYPE WTF\N";
		}
	}

	state = players[Me].GetMana() ? GameState::BATTLE : GameState::DRAFT;
}

void State::Think()
{
	if (state == GameState::DRAFT)
		m_Drafter.MakeChoice();
	else {
		vector<Card> playableCards;

		copy_if(myHand.begin(), myHand.end(), back_inserter(playableCards), [&](Card card) {
			assert(card.location == Card::Location::InHand);
			auto& me = players[Me];
			if (card.cost <= me.GetMana()) {
				if (card.type == Card::Type::GreenItem) {
					if (!myBoard.empty()) {
						me.SpendMana(card.cost);
						card.location = Card::Location::OnBoard;
						return true;
					}
				}
				else if (card.type == Card::Type::Creature) {
					me.SpendMana(card.cost);
					card.location = Card::Location::OnBoard;
					return true;
				}
			}
			return false;
			});


		Action a;
		for (Card& card : playableCards) {
			if (card.type == Card::Type::Creature) {
				a.Summon(card.instanceId);


			}
			else if (card.type == Card::Type::GreenItem) {
				a.Use(card.instanceId, myBoard.front().instanceId);
				myBoard.front().attack += card.attack;
				myBoard.front().defense += card.defense;
			}
		}
		Battle();
	}

	cout << "\n";
}
