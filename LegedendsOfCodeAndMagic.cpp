#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <sstream>
#include<map>
#include<set>
#include<numeric>
#include<cassert>
#include<chrono>
#include<functional>
#include<ctime>
#include<cstring>
#include<list>
using namespace std;
using namespace std::chrono;
namespace GameConstants {
	const int maxHandCardsAmount = 8;
	const int maxBoardCardsAmount = 6;
	const int GoodCurve = 6;
	const float FaceAttackVal = 2.f;
	const float TurnDurr = 200.f;
	const uint32_t MaxPopCount = 2000;
}

class NoImplementedError {};
/**
 * Auto-generated code below aims at helping you parse
 * the standard input according to the problem statement.
 **/
 //bool operator == (const pair<int ,int > & firstPair, const pair<int, int > & secondPair) {
 //	return firstPair.first == secondPair.first && firstPair.second == secondPair.second;
 //}
static const char* g_encodingTable = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";
static char g_decodingTable[256] = { 0 };
static bool g_decodingInit = false;
high_resolution_clock::time_point turnStartPoint;
void trap() {
	*(int*)0 = 0;
}
struct BitStream {
	static const int MAX_SIZE = 1024;
	void initRead(const char* str);
	bool readBit();
	void writeBit(bool value);
	char buffer[MAX_SIZE] = { 0 };
	int iter = 0;
	int bitCount = 0;
	void incBitCount();
	void decode(int count);
	void encode();
	BitStream();
	void print(ostream& o);
	int readInt(int bits);
	void writeInt(int value, int bits);


};
int BitStream::readInt(int bits) {
	bool negative = readBit();

	int result = 0;
	for (int i = 0; i < bits; ++i) {
		result <<= 1;
		if (readBit()) result |= 1;
	}
	return negative ? -result : result;
}

void BitStream::writeInt(int value, int bits) {

	writeBit(value < 0);
	value = abs(value);
	int mask = 1 << (bits - 1);
	for (int i = 0; i < bits; ++i) {
		writeBit(value & mask);
		mask >>= 1;
	}
}

void BitStream::initRead(const char* str) {
	int count = strlen(str);
	strcpy(buffer, str);
	decode(count);
	iter = bitCount = 0;
}

void BitStream::print(ostream& o) {
	o << buffer;
}
BitStream::BitStream() {
	if (g_decodingInit) return;
	for (int i = 0; i < 64; ++i) {
		g_decodingTable[g_encodingTable[i]] = i;
	}
	g_decodingInit = true;

}
void BitStream::decode(int count) {
	for (int i = 0; i < count; i++) {
		char c = buffer[i];
		buffer[i] = g_decodingTable[c];
	}
}
void BitStream::encode() {
	while (bitCount != 0)writeBit(false);
	for (int i = 0; i < iter; ++i) {

		char c = buffer[i];
		buffer[i] = g_encodingTable[c];
	}

}
void BitStream::incBitCount() {

	bitCount++;
	if (bitCount >= 6) {
		iter++;
		if (iter >= MAX_SIZE) {
			cerr << "bitstream buffer full;";
			trap();
		}
		bitCount = 0;
	}
}
void BitStream::writeBit(bool value)
{
	auto& c = buffer[iter];
	c <<= 1;
	if (value) c |= 1;
	incBitCount();
}
bool BitStream::readBit() {
	auto& c = buffer[iter];
	bool value = c & (1 << (5 - bitCount));
	incBitCount();
	return value;

}
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
	Card() = default;
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
	void read(BitStream& bs);
	void write(BitStream& bs) const;

};
void Card::read(BitStream& bs) {
	cardId = bs.readInt(6);
	instanceId = bs.readInt(6);
	location = (Location)bs.readInt(2);
	type = (Type)bs.readInt(2);
	cost = bs.readInt(4);
	attack = bs.readInt(4);
	defense = bs.readInt(4);
	myHealthChange = bs.readInt(4);
	opponentHealthChange = bs.readInt(4);
	cardDraw = bs.readInt(3);

	if (bs.readBit()) {
		abilities |= Card::Abilities::Breakthrough;
	}
	if (bs.readBit()) {
		abilities |= Card::Abilities::Charge;
	}
	if (bs.readBit()) {
		abilities |= Card::Abilities::Drain;
	}
	if (bs.readBit()) {
		abilities |= Card::Abilities::Guard;
	}
	if (bs.readBit()) {
		abilities |= Card::Abilities::Lethal;
	}
	if (bs.readBit()) {
		abilities |= Card::Abilities::Ward;
	}
	canAttack = bs.readBit();

}
void Card::write(BitStream& bs) const{

	bs.writeInt(cardId, 6);
	bs.writeInt(instanceId, 6);
	bs.writeInt((int)location, 2);
	bs.writeInt((int)type, 2);
	bs.writeInt(cost, 4);
	bs.writeInt(attack, 4);
	bs.writeInt(defense, 4);
	bs.writeInt(myHealthChange, 4);
	bs.writeInt(opponentHealthChange, 4);
	bs.writeInt(cardDraw, 3);

	bs.writeBit(abilities & Card::Abilities::Breakthrough);
	bs.writeBit(abilities & Card::Abilities::Charge);
	bs.writeBit(abilities & Card::Abilities::Drain);
	bs.writeBit(abilities & Card::Abilities::Guard);
	bs.writeBit(abilities & Card::Abilities::Lethal);
	bs.writeBit(abilities & Card::Abilities::Ward);

	bs.writeBit(canAttack);
}
enum {
	Me = 0,
	Opponent = 1
};
class State;

class Action {
protected:
	State* m_State;
	Action(int Cost, State* state) : Cost(Cost), m_State(state) {
	};
	float value = 0;
	int Cost = 0;

public:
	Action(const Action&) = delete;
	Action(Action&&) = delete;
	int GetCost() const { return Cost; }
	int GetValue() const { return value; }
	virtual void Perform(ostream& s) = 0;
	friend class State;
	virtual bool IsEqual(const Action& another) const = 0;
	//virtual ~Action() {};
};

class ComplicatedAction final :public Action {
	vector<Action* > subActions;
public:
	ComplicatedAction(State* state, Action* first, Action* second);
	virtual void Perform(ostream& s)  override final;

	const vector<Action*>& GetSubs() const { return subActions; }
	virtual bool IsEqual(const Action& another) const override  final {
		for (auto& action : subActions) {
			if (action->IsEqual(another))
				return true;
		}
		return false;
	}
};

class SummonAction final : public Action {
private:
	Card* summonCard = nullptr;

public:
	friend class State;
	SummonAction(State* state, Card* card);
	virtual void Perform(ostream& s)  override final;
	virtual bool IsEqual(const Action& another) const override final {
		if (const SummonAction * castedToSummon = dynamic_cast<const SummonAction*>(&another)) {
			return this->summonCard == castedToSummon->summonCard;
		}
		else if (const ComplicatedAction * comp = dynamic_cast<const ComplicatedAction*>(&another)) {
			for (auto& sub : comp->GetSubs()) {
				if (IsEqual(*sub))
					return true;
			}
		}
		return false;
	}
};

class AttackAction final : public Action {
private:
	Card* myCard = nullptr;
	Card* enemyCard = nullptr;
	int notEnougthDamage = -100;
	bool needToErase = false;
	bool needToEraseEnemy = false;
public:
	AttackAction(State* state, Card* myCard, Card* enemyCard = nullptr);
	virtual void Perform(ostream& s) override final;
	virtual bool IsEqual(const Action& another) const override final {
		if (const AttackAction * ac = dynamic_cast<const AttackAction*>(&another)) {
			if (myCard == ac->myCard)
				return true;
			if (ac->enemyCard == enemyCard)
				if (ac->needToEraseEnemy || needToEraseEnemy)
					return true;
			return false;
		}
		else if (const ComplicatedAction * comp = dynamic_cast<const ComplicatedAction*>(&another)) {
			for (auto& kek : comp->GetSubs()) {
				if (IsEqual(*kek))
					return true;
			}
		}
		return false;
	}
	friend ComplicatedAction;
};

class PickAction final : public Action {
	int cardNum = -1;
public:
	PickAction(int cardNumber) :Action(0, nullptr), cardNum(cardNumber) {}
	virtual void Perform(ostream& s) override;
	virtual bool IsEqual(const Action& another) const override final { 
		assert(false);
		return false; }
};

class UseAction final : public Action {
private:
	Card* myCard;
	Card* target;
	bool needToRemoveEnemy = false;
	int notEnougthDamage = -100;
public:
	UseAction(State* state, Card* myCard, Card* enemyCard = nullptr);
	virtual void Perform(ostream& s) override;
	virtual bool IsEqual(const Action& another)const override final{
		if (const UseAction * ac = dynamic_cast<const UseAction*>(&another)) {
			return this->myCard == ac->myCard;
		}
		else if (const ComplicatedAction * comp = dynamic_cast<const ComplicatedAction*>(&another)) {
			for (auto& sub : comp->GetSubs()) {
				if (IsEqual(*sub))
					return true;
			}
		}
		return false;
	}
	friend ComplicatedAction;

};

class PassAction : public Action {
public:
	PassAction() :Action(0, nullptr) {};
	virtual void Perform(ostream& s) override;
	virtual bool IsEqual(const Action& another) const override { return false; }
};



class Calculator {
	const std::map<Card::Abilities, float> allAbilities = {
		{Card::Abilities::Breakthrough,0.5f},
	{Card::Abilities::Charge,0.7f},
	{Card::Abilities::Drain,0.5f},
	{Card::Abilities::Guard,1.2f},
	{Card::Abilities::Lethal,1.2f},
	{Card::Abilities::Ward,2.f} };

	map<Card::Type, function<float(const Card&)>> valueCalcFunctions;
	map<Card::Type, function<float(const Card&)>> powerCalcFunctions;


public:
	float CalcCardValue(const Card& card) const;
	float CalcCardPower(const Card& card) const;
	Calculator();
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
	int health = -1;
	int mana = -1;
	int deckCardsAmount = -1;
	int rune = -1;
	int draw = -1;

public:
	int GetMana() const { return mana; }
	void SpendMana(int minus) { mana -= minus; }
	int GetHealth() const { return health; }
	void DecreesHealth(int amount) { health -= amount; };
	Player(istream& i);
	Player() = default;
	void read(BitStream& bs);
	void write(BitStream& bs) const;

};
void Player::read(BitStream& bs) {
	health = bs.readInt(7);
	mana = bs.readInt(4);
	deckCardsAmount = bs.readInt(6);
	rune = bs.readInt(3);

	draw = bs.readInt(3);

}
void Player::write(BitStream& bs) const {
	bs.writeInt(health, 7);
	bs.writeInt(mana, 4);
	bs.writeInt(deckCardsAmount, 6);
	bs.writeInt(rune, 3);
	bs.writeInt(draw, 3);


}


class State {



	Player players[2];
	list<Card> myHand;
	list<Card> myBoard;
	list<Card> enemyBoard;

	GameState state;

	int CardsCount = -1;
public:
	inline GameState GetState() const { return state; }
	vector<Action*> GetAllPosibleActions();
	vector< Card*> GetEnemyTaunts();
	vector< Card*> GetSpells();
	int RemoveEnemyCard(Card* cardToRemove);
	int RemoveMyCardFromBoard(Card* cardToRemove);
	int RemoveMyCardFromHand(Card* cardToRemove);
	inline int GetCurrMana() const { return players[Me].GetMana(); }
	inline const list<Card>& GetMyHand() const { return myHand; }
	void read(BitStream& s);
	void write(BitStream& s) const;
	vector<Action*> CheckLetal();
	
	bool HaveWeCharge();
	State(istream& i);
	State() = default;
	friend class Action;
	friend class SummonAction;
	friend class UseAction;
	friend class AttackAction;
	friend class ComplicatedAction;
};


class Agent {
	State m_State;
	float Fitness(const Action* action);
	void Draft();
public:
	friend class State;
	void ChangeState(const State& state) {
		m_State = state;
	}
	State& GetState() { return m_State; }
	Agent(istream& a);
	Agent() = default;
	void Think();
	void StartSelection(vector<Action*>& population);
};


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



vector< Card*> State::GetEnemyTaunts() {
	vector< Card*> enemyTaunts;
	for (auto& card : enemyBoard)
		if (card.abilities & Card::Abilities::Guard)
			enemyTaunts.push_back(&card);

	return enemyTaunts;
}

vector< Card*> State::GetSpells()
{
	vector< Card*> spells;
	for (auto& card : myHand) {
		if (card.type == Card::Type::BlueItem ||
			card.type == Card::Type::RedItem ||
			card.type == Card::Type::GreenItem) {
			spells.push_back(&card);
		}
	}
	return spells;
}

vector<Action*> State::GetAllPosibleActions()
{
	vector<Action*> outActions;
	auto taunts = GetEnemyTaunts();
	if (!taunts.empty()) {

		for (auto& card : myBoard) {
			if (!card.canAttack)
				continue;
			for (auto& enemyTaunt : taunts) {
				outActions.push_back(new AttackAction(this, &card, enemyTaunt));
			}

		}
	}

	else {
		for (auto& card : myBoard) {
			if (!card.canAttack)
				continue;

			outActions.push_back(new AttackAction(this, &card));
			for (auto& enemyCard : enemyBoard) {
				outActions.push_back(new AttackAction(this, &card, &enemyCard));
			}

		}
	}

	for (auto& spell : GetSpells()) {
		if (spell->cost > players[Me].GetMana())
			continue;
		if (spell->type == Card::Type::GreenItem) {
			for (auto& myCard : myBoard) {
				outActions.push_back(new UseAction(this, spell, &myCard));
			}
		}
		else {
			for (auto& enemyCard : enemyBoard) {
				outActions.push_back(new UseAction(this, spell, &enemyCard));
			}
		}
	}
	for (auto& card : myHand) {
		if (card.type == Card::Type::Creature && card.cost <= players[Me].GetMana()) {
			outActions.push_back(new SummonAction(this, &card));
		}
	}


	vector<Action*> newOutActions;
	for_each(outActions.begin(), outActions.end(), [&newOutActions](Action* a) {
		if (a->GetValue() >= 0.f) {
			newOutActions.push_back(a);
		}
		});
	return newOutActions;
}
///////////////////////////STATE IMPLEMENTATION///////////////////////
bool State::HaveWeCharge() {
	for (auto& card : myBoard)
		if (card.abilities & Card::Abilities::Charge)
			return true;
	return false;
}
vector<Action*> State::CheckLetal() {
	if (!GetEnemyTaunts().empty())
		return {};
	int enemyHealth = players[Opponent].GetHealth();
	vector<Action*> actions;
	int damage = 0;
	for (auto card : myBoard) {
		damage += card.attack;
		actions.push_back(new AttackAction(this, &card));
	}
	for (auto& spell : myHand) {
		if (spell.cost > players[Me].GetMana()) continue;
		if (spell.type == Card::Type::BlueItem) {

			damage += spell.opponentHealthChange;
			damage -= spell.defense;
			actions.push_back(new UseAction(this, &spell));
		}
		else if (spell.type == Card::Type::RedItem) {
			if (!enemyBoard.empty()) {
				damage += spell.opponentHealthChange;
				actions.push_back(new UseAction(this, &spell, &enemyBoard.back()));
			}
			
		}
	}

	if (damage < enemyHealth) {
		actions.clear();
	}
	return actions;
}
int State::RemoveEnemyCard(Card* cardToRemove)
{
	list<Card>::iterator removeIt = enemyBoard.end();
	for (auto it = enemyBoard.begin(); it != enemyBoard.end(); ++it) {
		if (it->instanceId == cardToRemove->instanceId) {
			removeIt = it;
			break;
		}
	}
	if (removeIt != enemyBoard.end()) {
		enemyBoard.erase(removeIt);
	}
	return 1;
}

int State::RemoveMyCardFromBoard(Card* cardToRemove)
{

	list<Card>::iterator removeIt = myBoard.end();
	for (auto it = myBoard.begin(); it != myBoard.end(); ++it) {
		if (it->instanceId == cardToRemove->instanceId) {
			removeIt = it;
			break;
		}
	}
	if (removeIt != myBoard.end()) {
		myBoard.erase(removeIt);
	}

	return 1;
}
int State::RemoveMyCardFromHand(Card* cardToRemove) {

	list<Card>::iterator removeIt = myHand.end();
	for (auto it = myHand.begin(); it != myHand.end(); ++it) {
		if (it->instanceId == cardToRemove->instanceId) {
			removeIt = it;
		}
	}
	if (removeIt != myHand.end()) {
		myHand.erase(removeIt);
	}

	return 1;
}
void State::read(BitStream& s) {
	for (int i = 0; i < 2; ++i)
		players[i].read(s);

	state = players[Me].GetMana() ? GameState::BATTLE : GameState::DRAFT;

	CardsCount = s.readInt(8);
	for (int i = 0; i < CardsCount; ++i) {
		Card c;
		c.read(s);
		if (c.location == Card::Location::InHand)
			myHand.push_back(c);
		else if (c.location == Card::Location::OnBoard) {
			myBoard.push_back(c);
		}
		else if (c.location == Card::Location::OpponentsBoard) {
			enemyBoard.push_back(c);
		}
		else {
			cerr << "NO CARD TYPE WTF\N";
		}

	}


}
void State::write(BitStream& s) const {
	for (int i = 0; i < 2; ++i)
		players[i].write(s);
	s.writeInt(CardsCount, 8);
	for (auto& card : myHand)
		card.write(s);
	for (auto& card : myBoard)
		card.write(s);
	for (auto& card : enemyBoard)
		card.write(s);



}
State::State(istream& i)
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

	cin >> CardsCount; cin.ignore();

	for (int j = 0; j < CardsCount; j++) {
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



////////////////////////////////CALCULATOR IMPLEMENTATION/////////////////////////////

float Calculator::CalcCardPower(const Card& card) const
{
	return powerCalcFunctions.find(card.type)->second(card);
}

float Calculator::CalcCardValue(const Card& c) const
{

	auto it = valueCalcFunctions.find(c.type);
	if (it != valueCalcFunctions.end())
		return it->second(c);

	return 0.0f;
}

Calculator::Calculator()
{
	valueCalcFunctions.insert(make_pair(Card::Type::Creature, [&](const Card& c) {
		float value = 0.f;
		if (c.attack == 0 || c.cost == 0)
			return value;

		for (const auto& it : allAbilities)
			if (c.abilities & it.first)
				value += it.second;
		value += c.attack / c.cost;
		value += c.defense / c.cost;
		value += c.cardDraw;
		value += c.myHealthChange / 4;
		value += c.opponentHealthChange / 4;
		/*	if (c.cost < 2)
				value -= 0.7f;*/
		if (c.defense / c.attack >= 3 || c.attack / c.defense >= 3)
			value -= 1.5;
		if (c.cost >= 8)
			value -= 0.5f;
		return value;
		}));


	valueCalcFunctions.insert(make_pair(Card::Type::BlueItem, [&](const Card& c) {
		float value = 0.f;


		value += abs(c.defense) * 0.75;
		value += c.cardDraw * 1.5;
		value += c.myHealthChange / 2;
		value += c.opponentHealthChange / 2;
		value += 1 / c.cost;
		value -= 1.5f;
		return value;
		}));



	valueCalcFunctions.insert(make_pair(Card::Type::GreenItem, [&](const Card& c) {
		float value = 0.f;
		if (c.cost == 0.f || c.attack == 0.f || c.defense == 0.f)
			return value;
		for (const auto& it : allAbilities)
			if (c.abilities & it.first)
				value += it.second;
		value += c.attack / c.cost;
		value += c.defense / c.cost;
		value += c.cardDraw;
		value += c.myHealthChange / 4;
		value += c.opponentHealthChange / 4;
		if (c.cost < 3)
			value -= 1.f;
		return value;
		}));


	valueCalcFunctions.insert(make_pair(Card::Type::RedItem, [&](const Card& c) {
		float value = 0.f;
		if (c.cost == 0 || c.defense == 0)
			return value;
		for (const auto& it : allAbilities)
			if (c.abilities & it.first) {
				value += 0.5;
				break;
			}

		value += (c.attack * 2) / c.cost;
		value += abs(c.defense * 2) / c.cost;
		value += c.cardDraw * 1.5f;
		value += c.myHealthChange / 4;
		value += c.opponentHealthChange / 4;
		if (c.cost < 3)
			value -= 1.f;
		value -= 2.8f;
		return value;
		}));


	powerCalcFunctions.insert(make_pair(Card::Type::Creature, [&](const Card& c) {
		float value = 0.f;
		for (const auto& it : allAbilities)
			if (c.abilities & it.first)
				value += it.second * (c.cost ? c.cost : 1.F);
		value += c.attack;
		value += c.defense;
		return value;
		}));

	powerCalcFunctions.insert(make_pair(Card::Type::BlueItem, [&](const Card& c) {
		float value = 0.f;


		value += abs(c.defense) * 0.75;
		value += c.cardDraw * 1.5;
		value += c.myHealthChange / 2;
		value += c.opponentHealthChange / 2;
		value += 1 / c.cost;
		return value *= 2;
		}));

	powerCalcFunctions.insert(make_pair(Card::Type::GreenItem, [&](const Card& c) {
		float value = 0.f;
		for (const auto& it : allAbilities)
			if (c.abilities & it.first)
				value += it.second;
		value += c.attack / c.cost;
		value += c.defense / c.cost;
		value += c.cardDraw;
		value += c.myHealthChange / 4;
		value += c.opponentHealthChange / 4;
		if (c.cost < 3)
			value -= 1.f;
		value += 0.75;
		return value * 2;
		}));

	powerCalcFunctions.insert(make_pair(Card::Type::RedItem, [&](const Card& c) {
		float value = 0.f;
		for (const auto& it : allAbilities)
			if (c.abilities & it.first) {
				value += 2;
				break;
			}

		value += (c.attack * 2) / c.cost;
		value += abs(c.defense * 2) / c.cost;
		value += c.cardDraw * 1.5f;
		value += c.myHealthChange / 4;
		value += c.opponentHealthChange / 4;
		if (c.cost < 3)
			value -= 1.f;
		return value;
		}));


}
/////////////////////////////AGENT IMPLEMENTATION/////////////////////////////
float Agent::Fitness(const Action* action)
{
	return action->GetValue();
}

void Agent::Draft()
{
	float biggestVal = -1.f;
	int cardToPlay = 0;
	Calculator calc;
	int i = 0;
	for (auto& elem : m_State.GetMyHand()) {
		float val = calc.CalcCardValue(elem);
		if (val >= biggestVal) {
			cardToPlay = i;
			biggestVal = val;
		}
		i++;

		cerr << "Card id " << elem.cardId << " card value " << val << '\n';
	}


	PickAction p(cardToPlay);
	p.Perform(cout);
}

Agent::Agent(istream& a) : m_State(a)
{

}

void Agent::Think()
{
	if (m_State.GetState() == GameState::DRAFT) {

		Draft();
		cout << '\n';
		return;
	}

	auto letal = m_State.CheckLetal();
	if (!letal.empty()) {
		for (auto* action : letal)
			action->Perform(cout);
		cout << '\n';
		return;
	}
	bool wasTaunts = !m_State.GetEnemyTaunts().empty();
	auto population = m_State.GetAllPosibleActions();
	if (population.empty()) {
		PassAction().Perform(cout);
		cout << '\n';
		return;
	}
	StartSelection(population);

	if (wasTaunts || m_State.HaveWeCharge()) {
		population = m_State.GetAllPosibleActions();

		if (!population.empty()) {
			StartSelection(population);
			letal = m_State.CheckLetal();
			if (!letal.empty()) {
				for (auto* action : letal)
					action->Perform(cout);
			
			}
		}

	}




	cout << '\n';


}
void Agent::StartSelection(vector<Action*>& population) {
	population.reserve(population.size() * 4);
	int generationCout = 0;
	sort(population.begin(), population.end(), [](const Action* const left, const Action* const right) {
		return left->GetValue() > right->GetValue();
		});
	while (generationCout < 30) {

		vector<Action* > bestActions;
		bool popIsBig = population.size() > 2;
		if (popIsBig) {
			bestActions = { population[0],population[1] };
		}

		int popSize = population.size();
		bool first = false;

		for (int i = (popIsBig ? 2 : 0); i < popSize; ++i) {

			Action* curr = population[i];



			Action* addAct = popIsBig ? bestActions[first] : population[first];//Action operator ==
			if ((*addAct).IsEqual(*curr)) continue;
			if (m_State.GetCurrMana() < addAct->GetCost() + curr->GetCost()) continue;
			if (population.size() >= GameConstants::MaxPopCount) break;
			population.push_back(new ComplicatedAction(&m_State, addAct, curr));
			first = !first;
		}
		generationCout++;

		sort(population.begin(), population.end(), [](const Action* const left, const Action* const right) {
			return left->GetValue() > right->GetValue();
			});
	}

	population.front()->Perform(cout);
	for (auto* p : population)
		delete p;
}


////////////////////////////////////////ACTIONS IMPLEMENTATION//////////////////////////////

AttackAction::AttackAction(State* state, Card* pMyCard, Card* pEnemyCard) : Action(0, state), myCard(pMyCard), enemyCard(pEnemyCard) {

	Calculator calc;
	value = 0.f;

	if (!enemyCard)
	{
		value = GameConstants::FaceAttackVal;
		notEnougthDamage = m_State->players[Opponent].GetHealth() - myCard->attack;
		return;
	}

	if (enemyCard->abilities & Card::Abilities::Ward) {
		if (myCard->attack != 0) {
			value += pEnemyCard->defense / myCard->attack;
			if (myCard->attack <= 2 && enemyCard->attack > 5)
				value += 4;
		}
	}
	else {
		if (myCard->attack >= enemyCard->defense || myCard->abilities & Card::Abilities::Lethal) {
			value += calc.CalcCardPower(*enemyCard);
			needToEraseEnemy = true;
			if (myCard->attack == pEnemyCard->attack && myCard->defense == pEnemyCard->defense) {
				value += 1.f;
			}
		}
		else {
			notEnougthDamage = enemyCard->defense - myCard->attack;
		}
	}


	if (enemyCard->abilities & Card::Abilities::Lethal ||
		(!(myCard->abilities & Card::Abilities::Ward) && enemyCard->attack >= myCard->defense)) {
		value -= calc.CalcCardPower(*myCard);
		needToErase = true;
	}

	if (myCard->abilities & Card::Abilities::Ward) {
		value += myCard->defense / 2.f + enemyCard->attack / 2.f;
	}

	
}

void AttackAction::Perform(ostream& s)
{
	assert(enemyCard);
	assert(myCard);
	s << "ATTACK " << myCard->cardId << " " << enemyCard << ";";
	if (needToErase) m_State->RemoveMyCardFromBoard(myCard);
	else myCard->canAttack = false;
	if (enemyCard && needToEraseEnemy)
		m_State->RemoveEnemyCard(enemyCard);
}

SummonAction::SummonAction(State* state, Card* card) : Action(card->cost, state), summonCard(card) {

	value += Calculator().CalcCardPower(*card);
}

void SummonAction::Perform(ostream& s)
{
	s << "SUMMON " << summonCard->cardId << ";";
	m_State->players[Me].SpendMana(summonCard->cost);
	summonCard->location = Card::Location::OnBoard;
	if (summonCard->abilities & Card::Abilities::Charge)
		summonCard->canAttack = true;
	else
		summonCard->canAttack = false;
	m_State->myBoard.push_back(*summonCard);

	m_State->RemoveMyCardFromHand(summonCard);



}

UseAction::UseAction(State* state, Card* myCard, Card* pTarget) : Action(myCard->cost, state), myCard(myCard), target(pTarget) {

	if (!target) {
		value = 0.f;
		notEnougthDamage = m_State->players[Opponent].GetHealth() + myCard->defense;
		return;
	}

	if (myCard->type == Card::Type::RedItem || myCard->type == Card::Type::BlueItem) {
		if (target->abilities & Card::Abilities::Ward) {
			value += target->defense / abs(myCard->defense);
		}
		else {
			if (myCard->cardId == 151) {
				Calculator calc;
				auto power = calc.CalcCardPower(*pTarget);
				if (power >= 15)
					value += 10.f;
			}
			else {

				float diff = myCard->defense + target->defense;
				if (diff < 0) {
					value += diff / 2;
					needToRemoveEnemy = true;
					if (target->attack > 6)
						value += 10.f;
				}
				else if (!diff) {
					value += 1.5;
					needToRemoveEnemy = true;
					if (target->attack > 6)
						value += 10.f;
				}
				else {
					value -= myCard->defense;
					notEnougthDamage = diff;
				}
			}
		}
	}
	else {
		value = 3.f;
		if (target->abilities & Card::Abilities::Guard && m_State->players[Me].GetHealth() < 15)
			value += 10.f;

	}

}

void UseAction::Perform(ostream& s)
{
	assert(myCard);
	assert(target);
	s << "USE " << myCard->instanceId << " " << target->instanceId << ";";
	if (myCard)
		m_State->RemoveMyCardFromHand(this->myCard);
	if (target) {
		target->attack += myCard->attack;
		target->defense += myCard->defense;
	}
	else {
		m_State->players[Opponent].DecreesHealth(-myCard->defense);
	}
		
	if (needToRemoveEnemy && target)
		m_State->RemoveEnemyCard(target);

}

ComplicatedAction::ComplicatedAction(State* state, Action* first, Action* second) : Action(first->GetCost() + second->GetCost(), state)
{

	value = first->GetValue() + second->GetValue();
	Calculator calc;
	value = first->GetValue() + second->GetValue();
	if (AttackAction * firstAttack = dynamic_cast<AttackAction*> (first)) {
		if (AttackAction * secondAttack = dynamic_cast<AttackAction*> (second)) {
			if (firstAttack->enemyCard == secondAttack->enemyCard) {
				if (firstAttack->notEnougthDamage != -100 || secondAttack->notEnougthDamage != -100) {
					if (firstAttack->enemyCard) {
						if ((firstAttack->myCard->attack + secondAttack->myCard->attack) >= firstAttack->enemyCard->defense) {
							value += calc.CalcCardPower(*firstAttack->enemyCard);
						}
					}
					else {
						if ((firstAttack->myCard->attack + secondAttack->myCard->attack) >= m_State->players[Opponent].GetHealth())
						{
							value += 100;
						}
					}

				}
			}
		}
		else if (UseAction * secondUse = dynamic_cast<UseAction*> (second)) {
			if (firstAttack->enemyCard == secondUse->target) {
				if (firstAttack->notEnougthDamage != -100 || secondUse->notEnougthDamage != -100) {
					if (firstAttack->enemyCard) {
						if ((firstAttack->myCard->attack - secondUse->myCard->defense) >= firstAttack->enemyCard->defense) {
							value += calc.CalcCardPower(*firstAttack->enemyCard);
						}
					}
					else {
						if ((firstAttack->myCard->attack - secondUse->myCard->defense) >= m_State->players[Opponent].GetHealth())
						{
							value += 100;
						}
					}

				}
			}
		}
	}
	else if (UseAction * firstUse = dynamic_cast<UseAction*> (first)) {
		if (AttackAction * secondAttack = dynamic_cast<AttackAction*> (second)) {
			if (firstUse->target == secondAttack->enemyCard) {
				if (firstUse->notEnougthDamage != -100 || secondAttack->notEnougthDamage != -100) {
					if (firstUse->target) {
						if ((-firstUse->myCard->defense + secondAttack->myCard->attack) >= firstUse->target->defense) {
							value += calc.CalcCardPower(*firstUse->target);
						}
					}
					else {
						if ((-firstUse->myCard->defense + secondAttack->myCard->attack) >= m_State->players[Opponent].GetHealth())
						{
							value += 100;
						}
					}

				}
			}
		}
		else if (UseAction * secondUse = dynamic_cast<UseAction*> (second)) {
			if (firstUse->target == secondUse->target) {
				if (firstUse->notEnougthDamage != -100 || secondUse->notEnougthDamage != -100) {
					if (firstUse->target) {
						if ((-firstUse->myCard->defense + -secondUse->myCard->defense) >= firstUse->target->defense) {
							value += calc.CalcCardPower(*firstUse->target);
						}
					}
					else {
						if ((-firstUse->myCard->defense + -secondUse->myCard->defense) >= m_State->players[Opponent].GetHealth())
						{
							value += 100;
						}
					}
				}
			}
		}
	}

	subActions.push_back(first);
	subActions.push_back(second);


}

void ComplicatedAction::Perform(ostream& s)
{
	for (const auto& action : subActions)
		action->Perform(s);
}

void PassAction::Perform(ostream& s)
{
	s << "PASS";

}

void PickAction::Perform(ostream& s)
{
	s << "PICK " << cardNum;
}



int main()
{
	/*turnStartPoint = high_resolution_clock::now();
	BitStream bs;
	bs.initRead("B62145O70GAN3X51C00_7r8P0J6000vM400003yS0a228000hnf0WX0028");
	State s;
	s.read(bs);
	Agent a;
	a.ChangeState(s);
	a.Think();
	high_resolution_clock::time_point turnEnd = high_resolution_clock::now();
	cerr << "Turn time " << duration_cast<milliseconds>(turnEnd - turnStartPoint).count() << '\n';*/


	while (1) {
		turnStartPoint = high_resolution_clock::now();
		Agent s(cin);
		BitStream bs;
		s.GetState().write(bs);
		bs.encode();
		cerr << "BitStream ";
		bs.print(cerr);
		cerr << '\n';
		s.Think();

		high_resolution_clock::time_point turnEnd = high_resolution_clock::now();
		cerr << "Turn time " << duration_cast<milliseconds>(turnEnd - turnStartPoint).count() << '\n';

	}
}

// taunts problem
// time resource problem need optimize
//////////////////////////////////TODO/////////////////
//better to place several small minoin then big one which can be easily killed by existing enemy minion
//bad spells sometimes