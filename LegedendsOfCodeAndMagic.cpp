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
static const char * g_encodingTable = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz-_";
static char g_decodingTable[256] = {0};
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
	char buffer[MAX_SIZE] = {0};
	int iter = 0;
	int bitCount = 0;
	void incBitCount();
	void decode(int count);
	void encode();
	BitStream();
	void print(ostream& o);
	int readInt(int bits);
	void writeInt(int value,int bits);


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

void BitStream::print(ostream & o) {
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
void BitStream::encode(){
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
	bool value = c& (1 << (5 - bitCount));
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
	void write(BitStream& bs);

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
void Card::write(BitStream& bs) {

	bs.writeInt(cardId, 6);
	bs.writeInt(instanceId,6);
	bs.writeInt((int)location,2);
	bs.writeInt((int)type ,2);
	bs.writeInt(cost ,4);
	bs.writeInt(attack,4);
	bs.writeInt(defense ,4);
	bs.writeInt(myHealthChange,4);
	bs.writeInt(opponentHealthChange,4);
	bs.writeInt(cardDraw ,3);

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
class ComplicatedAction;
class Action {
protected:
	static uint32_t s_ActionsCount;
	State* m_State;
	Action(int Cost,State*state) : Cost(Cost),m_State(state) {
		id = s_ActionsCount++;
	};
	float value = 0;
	int Cost = 0;
	int id;
public:
	Action(const Action&) = delete;
	Action(Action&&) = delete;
	inline int GetId() const { return id; }
	static void ResetCount() { s_ActionsCount = 0; }
	int GetCost() const { return Cost; }
	int GetValue() const { return value; }
	virtual void Perform(ostream& s)  = 0;
	friend class State;
	virtual bool IsEqual(Action & another) = 0;
	//virtual ~Action() {};
};

class ComplicatedAction :public Action {
	vector<Action* > subActions;
public:
	ComplicatedAction(State* state, Action* first, Action* second);
	virtual void Perform(ostream& s)  override;

	const vector<Action*>& GetSubs() const { return subActions; }
	virtual bool IsEqual(Action& another) override {
		for (auto& action : subActions) {
			if (action->IsEqual(another))
				return true;
		}
		return false;
	}
};

uint32_t Action::s_ActionsCount = 0;
class SummonAction : public Action {
private:
	 Card* summonCard = nullptr;
	 int summonCardId = -1;
public:
	friend class State;
	SummonAction(State* state, Card* card);
	virtual void Perform(ostream& s)  override;
	virtual bool IsEqual(Action& another) override {
		if (SummonAction * ac = dynamic_cast<SummonAction*>(&another)) {
			return this->summonCardId == ac->summonCardId;
		}
		else if (ComplicatedAction * comp = dynamic_cast<ComplicatedAction*>(&another)) {
			for (auto& kek : comp->GetSubs()) {
				if (IsEqual(*kek))
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

	 bool needToErase = false;
	 bool needToEraseEnemy = false;
	 int cardInstanceID = -1;
	 int enemyInstanceID = -1;
public:
	AttackAction(State * state, Card* myCard,  Card* enemyCard = nullptr);
	virtual void Perform(ostream& s) override;
	int GetCardInstanceID() const { return cardInstanceID; }
	virtual bool IsEqual(Action& another) override {
		if (AttackAction * ac = dynamic_cast<AttackAction*>(&another)) {
			return this->cardInstanceID == ac->cardInstanceID;
		}
		else if (ComplicatedAction * comp = dynamic_cast<ComplicatedAction*>(&another)) {
			for (auto& kek : comp->GetSubs()) {
				if (IsEqual(*kek))
					return true;
			}
		}
		return false;
	}
};

class PickAction : public Action {
	int cardNum = -1;
public:
	PickAction(int cardNumber) :Action(0,nullptr), cardNum(cardNumber) {}
	virtual void Perform(ostream& s) override;
	virtual bool IsEqual(Action& another) override { return id == another.GetId(); }
};

class UseAction : public Action {
private:
	 Card* myCard;
	 Card* target;
	 int myCardInstanceID = -1;
	 int enemyCardInstanceID = -1;
	 bool needToRemoveEnemy = false;
public:
	UseAction(State * state, Card* myCard,  Card* enemyCard = nullptr);
	virtual void Perform(ostream& s) override;
	int GetCardInstanceID() const { return myCardInstanceID; }
	virtual bool IsEqual(Action& another) override {
		if (UseAction * ac = dynamic_cast<UseAction*>(&another)) {
			return this->myCardInstanceID == ac->myCardInstanceID;
		}
		else if (ComplicatedAction * comp = dynamic_cast<ComplicatedAction*>(&another)) {
			for (auto& kek : comp->GetSubs()) {
				if (IsEqual(*kek))
					return true;
			}
		}
		return false;
	}
};

class PassAction : public Action {
public:
	PassAction() :Action(0,nullptr){};
	virtual void Perform(ostream& s) override;
	virtual bool IsEqual(Action& another) override { return false; }
};



class Calculator {
	const std::map<Card::Abilities, float> allAbilities = {
		{Card::Abilities::Breakthrough,0.3f},
	{Card::Abilities::Charge,0.7f},
	{Card::Abilities::Drain,0.3f},
	{Card::Abilities::Guard,1.f},
	{Card::Abilities::Lethal,1.2f},
	{Card::Abilities::Ward,1.5f} };
	
	map<Card::Type, function<float(const Card&)>> valueCalcFunctions;
	map<Card::Type, function<float(const Card&)>> powerCalcFunctions;
	
	
public:
	float CalcCardValue(const Card& card) const;
	float CalcCardPower(const Card& card) const;
//	float CalcActionValue(const Action& action) const { return 0.f; };
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
	Player(istream& i);
	Player() = default;
	void read(BitStream& bs);
	void write(BitStream& bs);

};
void Player::read(BitStream& bs) {
	health= bs.readInt(7);
	mana = bs.readInt(4);
	deckCardsAmount = bs.readInt(6);
	rune = bs.readInt(3);

	draw = bs.readInt(3);

}
void Player::write(BitStream& bs) {
	bs.writeInt(health,7);
	bs.writeInt(mana,4);
	bs.writeInt(deckCardsAmount,6);
	bs.writeInt(rune,3);
	bs.writeInt(draw,3);


}


class State {
	

	
	Player players[2];
	vector<Card> myHand;
	vector<Card> myBoard;
	vector<Card> enemyBoard;

	
	int CardsCount;
public:
	inline GameState GetState() const { return state; }
	vector<Action*> GetAllPosibleActions();
	vector< Card*> GetEnemyTaunts() ;
	vector< Card*> GetSpells() ;
	int RemoveEnemyCard(int instanceId);
	int RemoveMyCard(int instanceId);
	inline int GetCurrMana() const { return players[Me].GetMana(); }
	inline const vector<Card>& GetMyHand() const { return myHand; }
	void read(BitStream& s);
	void write(BitStream& s);
	vector<Action*> CheckLetal();
	GameState state;
	State(istream& i);
	State() = default;
	friend class Action;
	friend class SummonAction;
	friend class UseAction;
};


class Agent {

	struct DNA {
		int Fitness;
		Action* action;
	};
	State m_State;
	
	float Fitness(const Action * action);
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



vector< Card*> State::GetEnemyTaunts()  {
	vector< Card*> enemyTaunts;
	for ( auto& card : enemyBoard)
		if (card.abilities & Card::Abilities::Guard)
			enemyTaunts.push_back(&card);

	return enemyTaunts;
}

vector< Card*> State::GetSpells() 
{
	vector< Card*> spells;
	for ( auto& card : myHand) {
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
		
		for ( auto& card : myBoard) {
			if (!card.canAttack)
				continue;
			for ( auto& enemyTaunt : taunts) {
				outActions.push_back(new AttackAction(this,&card, enemyTaunt));
			}

		}
	}

	else {
		for ( auto& card : myBoard) {
			if (!card.canAttack)
				continue;

			outActions.push_back(new AttackAction(this,&card));
			for ( auto& enemyCard : enemyBoard) {
				outActions.push_back(new AttackAction(this,&card, &enemyCard));
			}

		}
	}

	for ( auto& spell : GetSpells()) {
		if (spell->cost > players[Me].GetMana())
			continue;
		if (spell->type == Card::Type::GreenItem) {
			for ( auto& myCard : myBoard) {
				outActions.push_back(new UseAction(this,spell,&myCard));
			}
		}
		else {
			for ( auto& enemyCard : enemyBoard) {
				outActions.push_back(new UseAction(this,spell, &enemyCard));
			}
		}
	}
	for ( auto& card : myHand) {
		if (card.type == Card::Type::Creature && card.cost <= players[Me].GetMana()) {
			outActions.push_back(new SummonAction(this,&card));
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
vector<Action*> State::CheckLetal() {
	if (!GetEnemyTaunts().empty())
		return {};
	int enemyHealth = players[Opponent].GetHealth();
	vector<Action*> actions;
	int damage = 0;
	for ( auto card : myBoard) {
		damage += card.attack;
		actions.push_back(new AttackAction(this,&card));
	}

	for ( auto& spell : myHand) {
		if (spell.cost > players[Me].GetMana()) continue;
		if (spell.type == Card::Type::BlueItem) {
			
			damage += spell.opponentHealthChange;
			damage -= spell.defense;
			actions.push_back(new UseAction(this,&spell));
		}
		else if (spell.type == Card::Type::RedItem) {
			if(!enemyBoard.empty())
			damage += spell.opponentHealthChange;
			actions.push_back(new UseAction(this,&spell,&enemyBoard.back()));
		}
	}

	if (damage < enemyHealth) {
		actions.clear();
	}
	return actions;
}
int State::RemoveEnemyCard(int instanceId)
{
	int index = -1;
	for (uint32_t i = 0; i < enemyBoard.size(); ++i)
		if (enemyBoard[i].instanceId == instanceId)
			index = i;

	if (index != -1)
		enemyBoard.erase(enemyBoard.begin() + index);
	return 1;
}

int State::RemoveMyCard(int instanceId)
{
	int index = -1;
	for (uint32_t i = 0; i < myBoard.size(); ++i)
		if (myBoard[i].instanceId == instanceId)
			index = i;

	if (index != -1)
		myBoard.erase(myBoard.begin() + index);
	return 1;
}
void State::read(BitStream& s) {
	for (int i = 0; i < 2; ++i)
		players[i].read(s);

	state = players[Me].GetMana()?	  GameState::BATTLE : GameState::DRAFT;

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
void State::write(BitStream& s) {
	for(int i = 0;i <2;++i)
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


////////////////////////////////CALCULATOR IMPLEMENTATION/////////////////////////////
Calculator::Calculator()
{
	valueCalcFunctions.insert(make_pair(Card::Type::Creature, [&](const Card & c) {
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
		if (c.cost < 3)
			value -= 1.f;
		if (c.defense / c.attack >= 3 || c.attack / c.defense >= 3)
			value -= 1.5;
		if (c.cost >= 8)
				value -= 0.7f;
		return value;
		}));


	valueCalcFunctions.insert(make_pair(Card::Type::BlueItem, [&](const Card& c) {
		float value = 0.f;


		value += abs(c.defense) * 0.75;
		value += c.cardDraw * 1.5;
		value += c.myHealthChange / 2;
		value += c.opponentHealthChange / 2;
		value += 1 / c.cost;
		return value;
		}));



	valueCalcFunctions.insert(make_pair(Card::Type::GreenItem, [&](const Card& c) {
		float value = 0.f;
		if (c.cost == 0.f)
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
		value += 0.75;
		return value;
		}));


	valueCalcFunctions.insert(make_pair(Card::Type::RedItem, [&](const Card& c) {
		float value = 0.f;
		for (const auto& it : allAbilities)
			if (c.abilities & it.first) {
				value += 0.5;
				break;
			}
		if (c.cost == 0)
			return value;
		value += (c.attack * 2) / c.cost;
		value += abs(c.defense * 2) / c.cost;
		value += c.cardDraw * 1.5f;
		value += c.myHealthChange / 4;
		value += c.opponentHealthChange / 4;
		if (c.cost < 3)
			value -= 1.f;
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
		return value *=2;
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
		return value*2;
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
	for (int i = 0; i < m_State.GetMyHand().size();++i) {

		float val = calc.CalcCardValue(m_State.GetMyHand()[i]);
		if (val >= biggestVal) {
			cardToPlay = i;
			biggestVal = val;
		}
	}


	PickAction p(cardToPlay);
	p.Perform(cout);
}

Agent::Agent(istream& a) : m_State(a)
{

}

void Agent::Think()
{
	if (m_State.state == GameState::DRAFT) {
		cerr << "In Draft\n";
		Draft();
		cout << '\n';
		return;
	}
	//auto kek = m_State.CheckLetal();
	//if (!kek.empty()) {
	//	for (auto& lol : kek)
	//		lol->Perform(cout);
	//	return;
	//}
	bool wasTaunts = !m_State.GetEnemyTaunts().empty();
	auto population = m_State.GetAllPosibleActions();
	if (population.empty()) {
		PassAction().Perform(cout);
		cout << '\n';
		return;
	}
	StartSelection(population);

	if (wasTaunts) {
		population = m_State.GetAllPosibleActions();
		if(!population.empty()) { StartSelection(population); }
			
	}
	



	cout << '\n';
	

}
void Agent::StartSelection(vector<Action*>& population) {
	population.reserve(population.size() * 4);
	int generationCout = 0;
	high_resolution_clock::time_point currTime = high_resolution_clock::now();
	sort(population.begin(), population.end(), [](const Action* const left, const Action* const right) {
		return left->GetValue() > right->GetValue();
		});
	float lastIterationMaxFitness = 0;

	while (generationCout < 30) {
		//while(generationCout < 300) {

		vector<Action* > bestActions;
		bool popIsBig = population.size() > 2;
		if (popIsBig) {
			bestActions = { population[0],population[1] };
		}

		int popSize = population.size();

		for (int i = (popIsBig ? 2 : 0); i < popSize; ++i) {
			srand(NULL);
			Action* curr = population[i];
			int tempSel = 0;


			tempSel = rand() % (popIsBig ? bestActions.size() : population.size());

			Action* addAct = popIsBig ? bestActions[tempSel] : population[tempSel];//Action operator ==
			if ((*addAct).IsEqual(*curr)) continue;
			if (m_State.GetCurrMana() < addAct->GetCost() + curr->GetCost()) continue;
			if (population.size() >= GameConstants::MaxPopCount) break;
			population.push_back(new ComplicatedAction(&m_State,addAct, curr));
		}
		generationCout++;

		sort(population.begin(), population.end(), [](const Action* const left, const Action* const right) {
			return left->GetValue() > right->GetValue();
			});
			/*if (lastIterationMaxFitness == population.front()->GetValue())
				break;*/
			lastIterationMaxFitness = population.front()->GetValue();
		currTime = high_resolution_clock::now();
	}

	cerr << "gen: " << generationCout << '\n';
	population.front()->Perform(cout);
	for (auto* p : population)
		delete p;
}


////////////////////////////////////////ACTIONS IMPLEMENTATION//////////////////////////////

AttackAction::AttackAction(State* state, Card* pMyCard,  Card* pEnemyCard) : Action(0,state), myCard(pMyCard), enemyCard(pEnemyCard) {
	
	Calculator calc;
	value = 0.f;
	cardInstanceID = pMyCard->instanceId;
	enemyInstanceID = pEnemyCard ? pEnemyCard->instanceId : -1;
	if (!enemyCard)
	{
		value = GameConstants::FaceAttackVal;
		return;
	}
	
	if (enemyCard->abilities & Card::Abilities::Ward) {
		if (myCard->attack != 0) {
			value += pEnemyCard->defense / myCard->attack;
		}
	}
	else {
		if (myCard->attack >= enemyCard->defense || myCard->abilities & Card::Abilities::Lethal) {
			value += calc.CalcCardPower(*enemyCard);
			needToEraseEnemy = true;
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
	s << "ATTACK " << cardInstanceID << " " << enemyInstanceID << ";";
	if (needToErase) m_State->RemoveMyCard(cardInstanceID);
	if(enemyCard && needToEraseEnemy)
		m_State->RemoveEnemyCard(enemyInstanceID);
}

SummonAction::SummonAction(State* state,  Card* card) : Action(card->cost,state), summonCard(card) {
	


	this->summonCardId = card->instanceId;
	value += Calculator().CalcCardPower(*card);
}

void SummonAction::Perform(ostream& s) 
{
	s << "SUMMON " << summonCardId << ";";
	m_State->myBoard.push_back(*summonCard);
	int c = -1;
	for (int i = 0; i < m_State->myHand.size(); ++i) {
		if (m_State->myHand[i].instanceId == summonCardId)
			c = i;

	}
	if(c != -1)
		m_State->myHand.erase(m_State->myHand.begin() + c);

}

UseAction::UseAction(State* state, Card* myCard,  Card* pTarget) : Action(myCard->cost,  state ), myCard(myCard), target(pTarget) {

	myCardInstanceID = myCard->instanceId;
	enemyCardInstanceID = -1;
	if (pTarget)
		enemyCardInstanceID = pTarget->instanceId;
	
	if (!target)
		value = 0.f;
	if (myCard->type == Card::Type::RedItem || myCard->type == Card::Type::BlueItem) {
		if (target->abilities & Card::Abilities::Ward) {
			value += target->defense / abs(myCard->defense);
		}
		else {
			float diff = myCard->defense + target->defense;
			if (diff < 0) {
				value += diff / 2;
				needToRemoveEnemy = true;
			}
			else if (!diff) {
				value += 1.5;
				needToRemoveEnemy = true;
			}
			else {
				value -= myCard->defense;
			}
		}
	}
	else {
		value = 3.f;
	}

}

void UseAction::Perform(ostream& s) 
{

	s << "USE " << myCardInstanceID << " " <<enemyCardInstanceID << ";";
	int c = 0;
	for (int i = 0; i < m_State->myHand.size(); ++i) {
		if (m_State->myHand[i] == *myCard)
			c = i;

	}

	m_State->myHand.erase(m_State->myHand.begin() + c);
	if (needToRemoveEnemy)
		m_State->RemoveEnemyCard(enemyCardInstanceID);
}

ComplicatedAction::ComplicatedAction(State* state,	Action* first, Action* second) : Action(first->GetCost() + second->GetCost(),state)
{

	value = first->GetValue() + second->GetValue();
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
	s << "PICK " << cardNum ;
}



int main()
{
	/*turnStartPoint = high_resolution_clock::now();
	BitStream bs;
	bs.initRead("81614U4O4GEa0W68G02060O0GX14008A2G1Y000o3W7ES004FXO8n300004IG4GGW02R3K0280G40");
	State s;
	s.read(bs);
	Agent a;
	a.ChangeState(s);
	a.Think();
	high_resolution_clock::time_point turnEnd = high_resolution_clock::now();
	cerr << "Turn time " << duration_cast<milliseconds>(turnEnd - turnStartPoint).count() << '\n';
*/

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