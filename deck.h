#ifndef DECK_H_INCLUDED
#define DECK_H_INCLUDED

#include <deque>
#include <list>
#include <map>
#include <random>
#include <vector>
#include "tyrant.h"
#include "card.h"

class Cards;

//---------------------- $30 Deck: a commander + a sequence of cards -----------
// Can be shuffled.
// Implementations: random player and raid decks, ordered player decks.
//------------------------------------------------------------------------------
namespace DeckStrategy
{
enum DeckStrategy
{
    random,
    ordered,
    exact_ordered,
    num_deckstrategies
};
}
typedef void (*DeckDecoder)(const char* hash, std::vector<unsigned>& ids);
typedef void (*DeckEncoder)(std::stringstream &ios, const Card* commander, std::vector<const Card*> cards);
void hash_to_ids_wmt_b64(const char* hash, std::vector<unsigned>& ids);
void encode_deck_wmt_b64(std::stringstream &ios, const Card* commander, std::vector<const Card*> cards);
void hash_to_ids_ext_b64(const char* hash, std::vector<unsigned>& ids);
void encode_deck_ext_b64(std::stringstream &ios, const Card* commander, std::vector<const Card*> cards);
void hash_to_ids_ddd_b64(const char* hash, std::vector<unsigned>& ids);
void encode_deck_ddd_b64(std::stringstream &ios, const Card* commander, std::vector<const Card*> cards);
extern DeckDecoder hash_to_ids;
extern DeckEncoder encode_deck;

//------------------------------------------------------------------------------
// No support for ordered raid decks
class Deck
{
public:
    const Cards& all_cards;
    DeckType::DeckType decktype;
    unsigned id;
    std::string name;
    Effect effect; // for quests
    unsigned upgrade_points;
    unsigned upgrade_opportunities;
    DeckStrategy::DeckStrategy strategy;

    const Card* base_commander;
    std::vector<const Card*> cards;
    std::map<signed, char> card_marks;  // <positions of card, prefix mark>: -1 indicating the commander. E.g, used as a mark to be kept in attacking deck when optimizing.

    const Card* commander;
    std::deque<const Card*> shuffled_cards;

    // card id -> card order
    std::map<unsigned, std::list<unsigned>> order;
    std::vector<std::pair<unsigned, std::vector<const Card*>>> raid_cards;
    std::vector<const Card*> reward_cards;
    unsigned mission_req;

    std::string deck_string;
    std::vector<unsigned> given_hand;
    std::vector<const Card*> fort_cards;

    Deck(
        const Cards& all_cards_,
        DeckType::DeckType decktype_ = DeckType::deck,
        unsigned id_ = 0,
        std::string name_ = "",
        Effect effect_ = Effect::none,
        unsigned upgrade_points_ = 0,
        unsigned upgrade_opportunities_ = 0,
        DeckStrategy::DeckStrategy strategy_ = DeckStrategy::random) :
        all_cards(all_cards_),
        decktype(decktype_),
        id(id_),
        name(name_),
        effect(Effect::none),
        upgrade_points(upgrade_points_),
        upgrade_opportunities(upgrade_opportunities_),
        strategy(strategy_),
        base_commander(nullptr),
        commander(nullptr),
        mission_req(0)
    {
    }

    ~Deck() {}

    void set(
        const Card* commander_,
        const std::vector<const Card*>& cards_,
        std::vector<std::pair<unsigned, std::vector<const Card*>>> raid_cards_ = {},
        std::vector<const Card*> reward_cards_ = {},
        unsigned mission_req_ = 0)
    {
        base_commander = commander_;
        cards = std::vector<const Card*>(std::begin(cards_), std::end(cards_));
        raid_cards = std::vector<std::pair<unsigned, std::vector<const Card*>>>(raid_cards_);
        reward_cards = std::vector<const Card*>(reward_cards_);
        mission_req = mission_req_;
    }

    void set(const std::vector<unsigned>& ids, const std::map<signed, char> &marks);
    void set(const std::vector<unsigned>& ids)
    {
        std::map<signed, char> empty;
        set(ids, empty);
    }
    void set(const std::string& deck_string_);
    void resolve();
    void set_given_hand(const std::string& deck_string_);
    void set_forts(const std::string& deck_string_);

    Deck* clone() const;
    std::string hash() const;
    std::string short_description() const;
    std::string medium_description() const;
    std::string long_description() const;
    void show_upgrades(std::stringstream &ios, const Card* card, const char * leading_chars) const;
    const Card* next();
    const Card* upgrade_card(const Card* card, std::mt19937& re, unsigned &remaining_upgrade_points, unsigned &remaining_upgrade_opportunities);
    const Card* get_commander();
    void shuffle(std::mt19937& re);
    void place_at_bottom(const Card* card);
};

typedef std::map<std::string, long double> DeckList;
class Decks
{
public:
    std::list<Deck> decks;
    std::map<std::pair<DeckType::DeckType, unsigned>, Deck*> by_type_id;
    std::map<std::string, Deck*> by_name;
};

#endif
