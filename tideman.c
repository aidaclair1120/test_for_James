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
bool is_loop(int current_pair, int original_winner, int locked_index[], int locked_count);
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
void sort_pairs(void)
{
    // TODO
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
        // printf("pairs winner %i with index %i\n", temp_winner, i);
    }
    return;
}


bool is_loop(int current_pair, int original_winner, int locked_index[], int locked_count)
{
    //for each locked pair
    for (int i = 0; i < locked_count; i++)
    {
        //does this locked pair list my current loser as a winner
        if (pairs[locked_index[i]].loser == original_winner)
        {
            return true;
        }

        //start a recursion, following the trail
        if (pairs[current_pair].loser == pairs[i].winner)
        {
            if (is_loop(locked_index[i], original_winner, locked_index, locked_count))
            {
                return true;
            }
        }
    }
    return false;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    int locked_index[pair_count];
    int locked_count = 0;

    // for each pair[]
    for (int i = 0; i < pair_count; i++)
    {
        //if it will not cause a loop
        if (!is_loop(i, pairs[i].winner, locked_index, locked_count))
        {
            //lock this pair
            locked[pairs[i].winner][pairs[i].loser] = true;
            locked_index[locked_count] = i;
            locked_count++;
        }
    }
    return;
}

// Print the winner of the election
void print_winner(void)
{
    // figure out who is at the top of the tree

    int winnerer = find_winners();
    if (winnerer != -1)
    {
        printf("%s\n", candidates[winnerer]);
    }
    return;
}

int find_winners(void)
{
    int i;
    for (i = 0; i < candidate_count; i++)
    {
        bool found = true;
        for (int j = 0; j < candidate_count; j++)
        {
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
    return -1;
}