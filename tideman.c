#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
}
pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;
int winner;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool is_loop(int original_winner, int current_loser);
int find_winners(void);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}


// Update ranks given a new vote

// "The function takes arguments rank, name, and ranks.
// If name is a match for the name of a valid candidate,
// then you should update the ranks array to indicate that the voter has the candidate as their rank preference
// (where 0 is the first preference, 1 is the second preference, etc.)"

// "Recall that ranks[i] here represents the user’s ith preference."

// "The function should return true if the rank was successfully recorded,
// and false otherwise (if, for instance, name is not the name of one of the candidates)."

// "You may assume that no two candidates will have the same name."
bool vote(int rank, string name, int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }
    // if criteria not met
    return false;
}

// Update preferences given one voter's ranks

/// "The function is called once for each voter,
// and takes as argument the ranks array, (recall that ranks[i] is the voter’s ith preference,
// where ranks[0] is the first preference)."

// The function should update the global preferences array to add the current voter’s preferences.
// Recall that preferences[i][j] should represent the number of voters who prefer candidate i over candidate j.

// You may assume that every voter will rank each of the candidates.
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = (candidate_count - 1); j > i; j--)
        {
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other

// "The function should add all pairs of candidates where one candidate is preferred to the pairs array.
// A pair of candidates who are tied (one is not preferred over the other) should not be added to the array."

// "The function should update the global variable pair_count to be the number of pairs of candidates.
// (The pairs should thus all be stored between pairs[0] and pairs[pair_count - 1], inclusive)."
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                // printf("recorded pair %i\n", pairs[pair_count].winner);
                pair_count++;
                // printf("recorded index %i\n", pair_count);
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory

// "The function should sort the pairs array in decreasing order of strength of victory,
// where strength of victory is defined to be the number of voters who prefer the preferred candidate.
// If multiple pairs have the same strength of victory, you may assume that the order does not matter."
void sort_pairs(void)
{
    // pair sorted_pairs[pair_count];
    for (int i = 0; i < pair_count; i++)
    {
        int x_margin = 0;
        int x_index = i;
        for (int j = i; j < pair_count; j++)
        {
                if ((preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[i].winner][pairs[i].loser]) > x_margin)
                {
                x_margin = (preferences[pairs[j].winner][pairs[j].loser] - preferences[pairs[i].winner][pairs[i].loser]);
                x_index = j;
                }
        }
        int temp_winner = pairs[i].winner;
        int temp_loser = pairs[i].loser;
        pairs[i] = pairs[x_index];
        pairs[x_index].winner = temp_winner;
        pairs[x_index].loser = temp_loser;
        // printf("pairs winner %i, loser %i with index %i\n", pairs[i].winner, pairs[i].loser, i);
    }
    return;
}

// This is a function to support locked_pairs()
bool is_loop(int original_winner, int current_loser)
{
    if (original_winner == current_loser)
        {
            return true;
        }
    //for each locked pair
    for (int i = 0; i < candidate_count; i++)
    {
        //does this locked pair list my current loser as a winner
        if (locked[current_loser][i])
        {
            if (is_loop(original_winner, i))
            {
                return true;
            }
        }
    }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles (a cycle of locked in voting preferences)

// "The function should create the locked graph, adding all edges in decreasing order of victory strength so long as the edge would not create a cycle."
void lock_pairs(void)
{
    //int locked_index[pair_count];
    int locked_count = 0;

    // for each pair[]
    for (int i = 0; i < pair_count; i++)
    {
        //if it will not cause a loop
        if (!is_loop(pairs[i].winner, pairs[i].loser))
        {
            //lock this pair
            locked[pairs[i].winner][pairs[i].loser] = true;
            //printf("locked %i %i\n", pairs[i].winner, pairs[i].loser);
            locked_count++;
            //printf("locked_count %i\n", locked_count);
        }
    }
    return;
}

// Print the winner of the election

// "The function should print out the name of the candidate who is the source of the graph. You may assume there will not be more than one source."
void print_winner(void)
{
    // figure out who is at the top of the tree
    int x = find_winners();
    if (x != -1)
    {
        printf("%s\n", candidates[x]);
    }
    return;
}

// This is a function to support print_winner()
int find_winners(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        bool found = true;
        for (int j = 0; j < candidate_count; j++)
        {
            // for each j, see if there are any locked winners over i
            // if there are no locked winners over i, then i must be the ultimate winner, otherwise check the next i.
            if (locked[j][i])
            {
                found = false;
                break;
            }
        }
        if (found)
        {
            return i;
        }
    }
    // if no winners found, return -1
    return -1;
}






