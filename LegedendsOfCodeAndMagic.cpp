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
	string abilities;
	int myHealthChange;
	int opponentHealthChange;
	int cardDraw;
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

class Me : public Player {
	vector<Card> hand;
	vector<Card> board;
	vector<Card> opponentsBoardCopy;
	map<pair<int, int>, vector<Card>> boardCardsByStats;
	int cardsAmount = -1;
	vector<Action* > actions;
	void DraftThink();
	void BattleThink();
	GameState state = GameState::DRAFT;
	void CheckForTaunts();

	void Trade(Card & enemyCard);
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
	cin >> cardId >> instanceId >> nlocation >> cardType >>
		cost >> attack >> defense >> 
		abilities >> myHealthChange >> opponentHealthChange
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
			auto it = boardCardsByStats.find(make_pair(card.attack, card.defense));
			if (it == boardCardsByStats.end()) {
				boardCardsByStats.insert(make_pair(make_pair(card.attack, card.defense), vector<Card>{card}));
			}
			else {
				it->second.push_back(card);
			}
			
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
	actions.push_back(new ActionPick(FIRST));
}

void Me::BattleThink()
{

	CheckForTaunts();
	vector<Card> playableCards;
	copy_if(hand.begin(), hand.end(), back_inserter(playableCards), [&](Card card) {
		if (card.cost <= mana) {
			playableCards.push_back(card);
			mana -= card.cost;
			return true;
		}
		return false;
		});
	for_each(playableCards.begin(), playableCards.end(), [&](Card card) {
		actions.push_back(new ActionSummon(card.instanceId));
		});
	for (const auto& card : board) {

		actions.push_back(new ActionAttack(card.instanceId));
	}

	
}

void Me::CheckForTaunts()
{
	vector<Card> opTaunts;
	for (auto c : opponentsBoardCopy) {
		if (c.abilities.find('G') != string::npos) {
			opTaunts.push_back(c);
		}
	}


	for (auto& t : opTaunts)
		Trade(t);

}

void Me::Trade(Card& enemyCard)
{
	auto fIt = boardCardsByStats.find(make_pair(enemyCard.defense, enemyCard.attack -1));
	if (fIt != boardCardsByStats.end()) {
		Card trade = fIt->second.back();
		actions.push_back( new ActionAttack(trade.instanceId, enemyCard.instanceId));
		opponentsBoardCopy.erase(remove(opponentsBoardCopy.begin(), opponentsBoardCopy.end(), enemyCard));
		board.erase(remove(board.begin(), board.end(), trade));
		fIt->second.pop_back();
		return;
	}
	vector<Card> bestChoice;
	for_each(boardCardsByStats.begin(), boardCardsByStats.end(), [&](const  pair<pair<int,int>,vector<Card>> & p) {
		if (p.first.first >= enemyCard.defense) {
			for (const auto& card : p.second)
				bestChoice.push_back(card);
		}
	});


	if (!bestChoice.empty()) {
		Card* toTrade = nullptr;
		for (auto card : bestChoice) {
			if (card.defense > enemyCard.attack) {
				toTrade=&card;
				break;
			}
		}
		if (!toTrade) {
			for (auto card : bestChoice) {
				if (card.defense < enemyCard.attack) {
					toTrade = &card;
					break;
				}
			}
		}
		if (!toTrade) {
			toTrade = &bestChoice[0];
		}
		actions.push_back(new ActionAttack(toTrade->instanceId, enemyCard.instanceId));
		opponentsBoardCopy.erase(remove(opponentsBoardCopy.begin(), opponentsBoardCopy.end(), enemyCard));
		board.erase(remove(board.begin(), board.end(), *toTrade));
		
		auto it = boardCardsByStats.find({ toTrade->attack,toTrade->defense });
		it->second.erase(remove(it->second.begin(), it->second.end(), *toTrade));
		return;
	}


	for (auto card : board) {
		actions.push_back(new ActionAttack(card.instanceId, enemyCard.instanceId));
		enemyCard.defense -= card.attack;
		if (enemyCard.defense <= 0) {
			return;
		}
	}
}
