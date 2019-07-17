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
void trap() {
	*(int*)0 = 0;
}
struct BitStream {
	static const int MAX_SIZE = 512;
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
		OpponentsBoard = 0,
		InHand = 1,
		OnBoard = 2
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
	void read(BitStream& bs);
	void write(BitStream& bs);

};
void Card::read(BitStream& bs) {
	cardId = bs.readInt(6);
	instanceId = bs.readInt(6);
	location = (Location)bs.readInt(2);
	type = (Type)bs.readInt(2);

}
void Card::write(BitStream& bs) {

	
}
enum {
	Me = 0,
	Opponent = 1
};
namespace GameConstants {
	const int maxHandCardsAmount = 8;
	const int maxBoardCardsAmount = 6;
	const int GoodCurve = 6;
	const float FaceAttackVal = 2.f;
}


class Action {

protected:
	Action(int Cost) : Cost(Cost) {};
	float value = 0;
	int Cost = 0;
public:
	
	int GetCost() const { return Cost; }
	int GetValue() const { return value; }
	virtual void Perform(ostream& s) const = 0;

};

class SummonAction : public Action {
private:
	const Card* summonCard;
public:
	SummonAction(const Card* card);
	virtual void Perform(ostream& s) const override;
};

class AttackAction : public Action {
private:
	const Card* myCard;
	const Card* enemyCard;
public:
	AttackAction(const Card* myCard, const Card* enemyCard = nullptr);
	virtual void Perform(ostream& s)const override;
};


class PickAction : public Action {
	int cardNum;
public:
	PickAction(int cardNumber) :Action(0), cardNum(cardNumber) {}
	virtual void Perform(ostream& s)const override;
};

class UseAction : public Action {
private:
	const Card* myCard;
	const Card* target;
public:
	UseAction(const Card* myCard, const Card* enemyCard = nullptr);
	virtual void Perform(ostream& s)const override;
};

class PassAction : public Action {
public:
	PassAction() = default;
	virtual void Perform(ostream& s)const override;
};
class ComplicatedAction :public Action {
	vector<Action* > subActions;
public:
	ComplicatedAction(Action* first, Action* second);
	virtual void Perform(ostream& s)const override;
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
	

	GameState state;
	Player players[2];
	vector<Card> myHand;
	vector<Card> myBoard;
	vector<Card> enemyBoard;

	vector<const Card*> GetEnemyTaunts() const;
	vector<const Card*> GetSpells() const;
	
public:
	inline GameState GetState() const { return state; }
	vector<Action*> GetAllPosibleActions() const;
	int RemoveEnemyCard(Card& enemyCard);
	int RemoveMyCard(Card& myCard);
	inline int GetCurrMana() const { return players[Me].GetMana(); }
	inline const vector<Card>& GetMyHand() const { return myHand; }
	void read(BitStream& s);
	void write(BitStream& s);

	State(istream& i);
	State() = default;
};
void State::read(BitStream& s)	{
	for(int i = 0;i<2;++i)
		players[i].read(s);

	state = players[Me].GetMana() ? GameState::BATTLE : GameState::DRAFT;


}
void State::write(BitStream& s) {
	players[Me].write(s);
}

class Agent {

	struct DNA {
		int Fitness;
		Action* action;
	};
	State m_State;
	GameState m_GameState;
	float Fitness(const Action * action);
	void Draft();
public:
	State& GetState() { return m_State; }
	Agent(istream& a);
	void Think();
};

int main()
{

	int kek = 0;

	while (1) {
		high_resolution_clock::time_point p1 = high_resolution_clock::now();
		Agent s(cin);
		if (kek < 30) {
			cout << "PICK 0";
		}
		else {
			s.Think();
		}
		kek++;
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



vector<const Card*> State::GetEnemyTaunts() const {
	vector<const Card*> enemyTaunts;
	for (const auto& card : enemyBoard)
		if (card.abilities & Card::Abilities::Guard)
			enemyTaunts.push_back(&card);

	return enemyTaunts;
}

vector<const Card*> State::GetSpells() const
{

	vector<const Card*> spells;
	for (const auto& card : myHand) {
		if (card.type == Card::Type::BlueItem ||
			card.type == Card::Type::RedItem ||
			card.type == Card::Type::GreenItem) {
			spells.push_back(&card);
		}
	}
	return spells;
}

vector<Action*> State::GetAllPosibleActions() const
{
	vector<Action*> outActions;
	auto taunts = GetEnemyTaunts();
	if (!taunts.empty()) {
		
		for (const auto& card : myBoard) {
			if (!card.canAttack)
				continue;
			for (const auto& enemyTaunt : taunts) {
				outActions.push_back(new AttackAction(&card, enemyTaunt));
			}

		}
	}

	else {
		for (const auto& card : myBoard) {
			if (!card.canAttack)
				continue;

			outActions.push_back(new AttackAction(&card));
			for (const auto& enemyCard : enemyBoard) {
				outActions.push_back(new AttackAction(&card, &enemyCard));
			}

		}
	}

	for (const auto& spell : GetSpells()) {
		if (spell->type == Card::Type::GreenItem) {
			for (const auto& myCard : myBoard) {
				outActions.push_back(new UseAction(spell,&myCard));
			}
		}
		else {
			for (const auto& enemyCard : enemyBoard) {
				outActions.push_back(new UseAction(spell, &enemyCard));
			}
		}
	}
	for (const auto& card : myHand) {
		if (card.type == Card::Type::Creature && card.cost <= players[Me].GetMana()) {
			outActions.push_back(new SummonAction(&card));
		}
	}


	

	return outActions;
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
	valueCalcFunctions.insert(make_pair(Card::Type::Creature, [&](const Card & c) {
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


	powerCalcFunctions.insert(make_pair(Card::Type::Creature, [&](const Card& c) {
		float value = 0.f;
		for (const auto& it : allAbilities)
			if (c.abilities & it.first)
				value += it.second * c.cost;
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
	m_GameState = m_State.GetState();

}

void Agent::Think()
{
	if (m_GameState == GameState::DRAFT) {
		cerr << "In Draft\n";
		Draft();
		return;
	}
	auto population = m_State.GetAllPosibleActions();
	population.reserve(population.size() * 4);
	int generationCout = 0;
	while (generationCout < 14) {


		sort(population.begin(), population.end(), [](const Action * const left, const Action* const right) {
			return left->GetValue() > right->GetValue();
		});
		vector<Action* > bestActions;
		bool popIsBig = population.size() > 3;
		if (popIsBig) {
			bestActions = { population[0],population[1] ,population[2] };
		}

		for (int i = (popIsBig ? 2 : 0); i < population.size(); ++i) {
			srand(NULL);
			int tempSel = 0;
		
			
			tempSel = rand() % (popIsBig ? bestActions.size() :population.size());
		
			Action* addAct = popIsBig ? bestActions[tempSel] : population[tempSel];
			if (m_State.GetCurrMana() >= addAct->GetCost() + population[i]->GetCost()) {
				population.push_back(new ComplicatedAction(addAct, population[i]));
			}
		}
	}

	

}

AttackAction::AttackAction(const Card* pMyCard, const Card* pEnemyCard) : Action(0), myCard(pMyCard), enemyCard(pEnemyCard) {
	
	Calculator calc;
	value = 0.f;
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
		}
	}


	if (enemyCard->abilities & Card::Abilities::Lethal || 
		(!(myCard->abilities & Card::Abilities::Ward) && enemyCard->attack >= myCard->defense)) {
		value -= calc.CalcCardPower(*myCard);
	}

	if (myCard->abilities & Card::Abilities::Ward) {
		value += myCard->defense / 2.f + enemyCard->attack / 2.f;
	}



}

void AttackAction::Perform(ostream& s) const
{
	s << "ATTACK " << myCard->instanceId << " " << (enemyCard ? enemyCard->instanceId : -1) << ";";
}

SummonAction::SummonAction(const Card* card) : Action(card->cost), summonCard(card) {
	value += Calculator().CalcCardPower(*card);
}

void SummonAction::Perform(ostream& s) const
{
	s << "SUMMON " << summonCard->instanceId << ";";
}

UseAction::UseAction(const Card* myCard, const Card* pTarget) : Action(myCard->cost), myCard(myCard), target(pTarget) {

	if (!target)
		value = 0.f;
	if (myCard->type == Card::Type::RedItem || myCard->type == Card::Type::BlueItem) {
		if (target->abilities & Card::Abilities::Ward) {
			value += target->defense / abs(myCard->defense);
		}
		else {
			float diff = myCard->defense + target->defense;
			if (diff < 0) {

			}
			else if (!diff) {

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

void UseAction::Perform(ostream& s) const
{

	s << "USE " << myCard->instanceId << " " << (target ? target->instanceId : -1) << ";";
}

ComplicatedAction::ComplicatedAction(Action* first, Action* second) : Action(first->GetCost() + second->GetCost())
{

	value = first->GetValue() + second->GetValue();
}

void ComplicatedAction::Perform(ostream& s) const
{
	for (const auto& action : subActions)
		action->Perform(s);
}

void PassAction::Perform(ostream& s) const
{
	s << "PASS";

}

void PickAction::Perform(ostream& s) const
{
	s << "PICK " << cardNum;
}

