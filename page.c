#define _CRT_SECURE_NO_WARNINGS
#define MOVIES_DATA_FILE "moviesData.txt"
#define VOTING_DATA_FILE "votingData.txt"
#define RECOMMENDED_FILE "Recommendation.txt"
#define BUFFER_SIZE 1024
#define STUDIO_SIZE 30
#define COUNTRY_SIZE 15
#define LENGTH 30

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Definition of vote structure
typedef struct {
	int value; // the grade of the reviewer
	char* p2comment; // comment
	char country[COUNTRY_SIZE]; // origin country
}vote;

//Definition of movie structure
typedef struct {
	int id;                   // Identification number of the movie
	char* p2name;             // Name of the movie
	char* p2genre;            // Type of the movie
	char studio[STUDIO_SIZE]; // Name of the studio
	vote* p2list;             // pointer to a dynamic array of votes structures
	int year;                 // movie released year
	int Nvotes;               // Number of the votes for a movie
}movie;

/* Helper - go to the end of the input buffer(stdin)
 in order to fix the bug of \n being left in the input buffer when user presses enter.*/
void fixScanfBug()
{
	fseek(stdin, 0, SEEK_END);
}


/*Helper function to allocate dynamic memory for a string
str:pointer to string to be copied
function returns a pointer to the newly allocated memory containing the copied string.*/
char* turnStringDynamic(const char* str)
{
	char* pString = (char*)malloc((strlen(str) + 1) * sizeof(char));
	if (pString == NULL)
	{
		printf("Error, file not opened\n");
		exit(1); // Return Null in case memory allocation error
	}
	strcpy(pString, str);
	return pString;
}

//Helper function to find movie by Id
movie* findMovieById(const movie* moviesArray, int moviesArraySize, int movieId)
{
	movie* foundMovie = NULL;
	for (int i = 0; i < moviesArraySize; i++)
	{
		if (moviesArray[i].id == movieId) {
			foundMovie = &moviesArray[i];
			break;
		}
	}
	return foundMovie;
}

//Helper function to find movie by name
movie* findMovieByName(const movie* moviesArray, int moviesArraySize, char* movieName)
{
	movie* foundMovie = NULL;
	for (int i = 0; i < moviesArraySize; i++)
	{
		if (strcmp(moviesArray[i].p2name, movieName) == 0)
		{
			foundMovie = &moviesArray[i];
			break;
		}
	}
	return foundMovie;
}

//Function that counts the number of lines in a text file and returns the number of lines in the text file
//textFile: Name of the text file to cound lines in
int countLines(const char* textFile) // function that calculates the number of rows in a file
{
	int count = 0;  // count rows from 0
	char buffer[BUFFER_SIZE]; // using a buffer to store each line that been read from the file
	FILE* movieFile = fopen(textFile, "r");
	if (movieFile == NULL) { // checking if the file type is NULL, meaning not opening
		printf("File not opened\n");
		exit(1);
	}
	fgets(buffer, sizeof(buffer), movieFile);
	while (fgets(buffer, sizeof(buffer), movieFile) != NULL) { // read each line from the file into the buffer
		count++; // increment the line count for each line read
	}
	fclose(movieFile);
	return count;
}

//Function to read movies data from a file and populate an array of movies
void FromFile2Movies(const char* moviesDataFile, movie* movieArray, const int arraySize)
{
	FILE* movieFile = fopen(moviesDataFile, "r"); // open file to read
	char movieName[BUFFER_SIZE], genre[BUFFER_SIZE]; // Buffer to store each line read from the file
	if (movieFile == NULL) { // checking if the file type is NULL, meaning not opening
		printf("Error, file not opened\n");
		exit(1);
	}
	fgets(movieName, BUFFER_SIZE, movieFile);

	// format:m_id,movie_name,Genre,Lead Studio,Year
	// 4,When in Rome,Comedy,Disney,2010
	for (int i = 0; i < arraySize; i++)
	{
		fscanf(movieFile, "%d,%[^,],%[^,],%[^,],%d",
			&movieArray[i].id,
			movieName,
			genre,
			movieArray[i].studio,
			&movieArray[i].year);
		fixScanfBug();
		movieArray[i].p2name = turnStringDynamic(movieName); // Allocate dynamic memory for movie name
		movieArray[i].p2genre = turnStringDynamic(genre); // Allocate dynamic memory for genre
		movieArray[i].Nvotes = 0; // Initialize number of votes
		movieArray[i].p2list = NULL; // Initialize number of votes
	}
	fclose(movieFile); // close the file
}

//Function reads data from the voting file and fills in the voting lists for each movie in the movie array and returns void
// votingDataFile: Name of the viewer votes file
// movieArray: pointer to the array of movies
// arraySize: Size of the movie array
void FromFile2Votes(const char* votingDataFile, movie* moviesArray, const int moviesArraySize)
{
	FILE* votingFile = fopen(votingDataFile, "r"); // open the voting data file in order to read
	if (votingFile == NULL) { // checks if the file failed to open
		printf("File not opened\n");
		exit(1);
	}
	int votingFileLen = countLines(VOTING_DATA_FILE);

	char line[BUFFER_SIZE];
	fgets(line, BUFFER_SIZE, votingFile);

	for (int i = 0; i < votingFileLen; i++)
	{
		vote curVote;
		char curVoteComment[BUFFER_SIZE];

		int curVoteMovId = 0;
		fscanf(votingFile, "%d:%d:%[^:]:%[^\n]",
			&curVoteMovId,
			&curVote.value,
			curVote.country,
			curVoteComment);
		fixScanfBug();
		curVote.p2comment = turnStringDynamic(curVoteComment);

		movie* foundMovie = findMovieById(moviesArray, moviesArraySize, curVoteMovId);

		if (foundMovie == NULL) {
			printf("There is no movie with ID %d\n", curVoteMovId);
			continue;
		}

		int foundMovVotesNum = foundMovie->Nvotes;
		vote* pVotes = foundMovie->p2list;

		vote* pNewVotesList = realloc(pVotes, (foundMovVotesNum + 1) * sizeof(vote));

		if (pNewVotesList == NULL) { // Check if allocation work
			printf("Cannot allocate this memory");
			return 0;
		}

		foundMovie->p2list = pNewVotesList;
		foundMovie->p2list[foundMovVotesNum] = curVote;
		foundMovie->Nvotes++;
	}
	fclose(votingFile); // close the file
}

// Function added new movie to the movies array
int addMovie(movie** moviesPtrArr, int* moviesArrSize) {
	movie newMovie;
	char movieName[BUFFER_SIZE], genre[BUFFER_SIZE]; // Buffer to store each line read from the file
	printf("Enter movie fields : ID, Name, Genre, Studio, Year\n");
	scanf("%d %[^\n] %[^\n] %[^\n] %d",
		&newMovie.id,
		movieName,
		genre,
		newMovie.studio,
		&newMovie.year);
	fixScanfBug();
	newMovie.p2name = turnStringDynamic(movieName); // Allocate dynamic memory for movie name
	newMovie.p2genre = turnStringDynamic(genre); // Allocate dynamic memory for genre
	newMovie.Nvotes = 0; // Initialize number of votes
	newMovie.p2list = NULL; // Initialize number of votes

	int movWUserIdExists = 0;
	if (findMovieById(*moviesPtrArr, *moviesArrSize, newMovie.id) != NULL)
	{
		printf("movie with id %d already exists\n", newMovie.id);
		return 0;
	}

	movie* newMovies = realloc(*moviesPtrArr, ((*moviesArrSize) + 1) * sizeof(movie));
	if (newMovies == NULL) { // Check if allocation work
		printf("Cannot allocate this memory");
		return 0;
	}
	*moviesPtrArr = newMovies;
	(*moviesPtrArr)[*moviesArrSize] = newMovie;
	(*moviesArrSize)++;
	return 1;
}

int addVote(movie* moviesArray, int moviesArraySize, int movieId)
{
	movie* foundMovie = findMovieById(moviesArray, moviesArraySize, movieId); //Find movie by id
	if (foundMovie == NULL) {
		printf("There is no movie with this ID\n");
		return 0;
	}

	vote newVote;
	char comment[BUFFER_SIZE];
	printf("Enter vote fields : Value, Country, Comment\n");
	scanf("%d %[^\n] %[^\n]", &newVote.value, newVote.country, comment);
	fixScanfBug();
	newVote.p2comment = turnStringDynamic(comment); //After the user registers a response, allocate memory space according to the size of the comment

	//compare between user fields and 
	for (int i = 0; i < foundMovie->Nvotes; i++)
	{
		if (foundMovie->p2list[i].value == newVote.value &&
			strcmp(foundMovie->p2list[i].country, newVote.country) == 0 &&
			strcmp(foundMovie->p2list[i].p2comment, newVote.p2comment) == 0)
		{
			printf("a vote with value %d, country %s, comment %s for movie id %d already exists\n",
				newVote.value,
				newVote.country,
				newVote.p2comment,
				movieId);
			return 0;
		}
	}

	vote* newVotes = realloc(foundMovie->p2list, (foundMovie->Nvotes + 1) * sizeof(vote));
	if (newVotes == NULL) { // Check if allocation work
		printf("Cannot allocate this memory");
		return 0;
	}
	foundMovie->p2list = newVotes;
	foundMovie->p2list[foundMovie->Nvotes] = newVote;
	foundMovie->Nvotes++;
	return 1;
}

//This function compare between 2 genres and if it is the same genre, it shows the movie name */
void countGenre(const movie* moviesArray, const int moviesArraySize, const char genre[LENGTH])
{
	int isGenreExist = 0;

	for (int i = 0; i < moviesArraySize; i++)
	{
		if (strcmp(moviesArray[i].p2genre, genre) == 0) { 	//Compare between letters 
			isGenreExist = 1;
			printf("%s\n", moviesArray[i].p2name);
		}
	}

	if (!isGenreExist) { //If the genre not exist in the movies array
		printf("no movies of genre %s found\n", genre);
	}
}

void printValue(const movie* moviesArray, const int moviesArraySize, const int value, const char* country)
{
	int foundVoteValueOrCountry = 0;
	for (int i = 0; i < moviesArraySize; i++)
	{
		int movieHasVoteNeeded = 0;
		for (int j = 0; j < moviesArray[i].Nvotes; j++)
		{
			if (moviesArray[i].p2list[j].value == value ||
				strcmp(moviesArray[i].p2list[j].country, country) == 0)
			{
				foundVoteValueOrCountry = 1;
			}

			if (moviesArray[i].p2list[j].value == value &&
				strcmp(moviesArray[i].p2list[j].country, country) == 0)
			{
				movieHasVoteNeeded = 1;
			}
		}
		if (movieHasVoteNeeded)
			printf("%s\n", moviesArray[i].p2name);
	}

	if (!foundVoteValueOrCountry)
		printf("no votes with value %d or country %s were found\n", value, country);
}

int printVotes(movie* moviesArray, const int moviesArraySize, char* movieName)
{
	movie* pFoundMov = findMovieByName(moviesArray, moviesArraySize, movieName);
	if (pFoundMov == NULL)
		return -1;

	if (pFoundMov->Nvotes == 0)
		return 0;

	for (int i = 0; i < pFoundMov->Nvotes; i++)
	{
		printf("%s, %s\n",
			pFoundMov->p2list[i].country,
			pFoundMov->p2list[i].p2comment);
	}
	return 1;
}

int countCountry(const movie* moviesArray, const int moviesArraySize, const int year)
{
	char(*uniqueCountriesNames)[COUNTRY_SIZE] = NULL;
	int uniqueCountriesNamesLen = 0;

	for (int i = 0; i < moviesArraySize; i++) // movies
	{
		if (moviesArray[i].year == year)
		{
			for (int j = 0; j < moviesArray[i].Nvotes; j++) // current movie's votes
			{
				int curVoteCountryInUniqs = 0;
				char* voteCountry = moviesArray[i].p2list[j].country;

				// is current vote's country name in unique countries names array.
				for (int k = 0; k < uniqueCountriesNamesLen; k++)
				{
					if (strcmp(uniqueCountriesNames[k], voteCountry) == 0)
					{
						curVoteCountryInUniqs = 1;
						break;
					}
				}

				if (!curVoteCountryInUniqs)
				{
					char(*newUniqueCountriesNames)[COUNTRY_SIZE] =
						realloc(uniqueCountriesNames, (uniqueCountriesNamesLen + 1) * COUNTRY_SIZE);
					if (newUniqueCountriesNames == NULL) { // Check if allocation work
						printf("Cannot allocate this memory");
						return 0;
					}
					uniqueCountriesNames = newUniqueCountriesNames;
					strcpy(uniqueCountriesNames[uniqueCountriesNamesLen], voteCountry);
					uniqueCountriesNamesLen++;
				}
			}
		}
	}
	printf("number of unique countries who voted for movies in year %d is: %d\n",
		year, uniqueCountriesNamesLen);

	free(uniqueCountriesNames);
}

int maxByCountry(const movie* moviesArray, const int moviesArraySize)
{
	char(*countriesNames)[COUNTRY_SIZE] = NULL;
	int* countriesCommentsNum = NULL;
	int countriesNum = 0;

	for (int i = 0; i < moviesArraySize; i++) // All movies
	{
		for (int j = 0; j < moviesArray[i].Nvotes; j++) // current movie's votes
		{
			if (strcmp(moviesArray[i].p2list[j].p2comment, "-") != 0)
			{
				int foundCountryIndex = -1;
				for (int k = 0; k < countriesNum; k++)
				{
					if (strcmp(moviesArray[i].p2list[j].country, countriesNames[k]) == 0)
					{
						foundCountryIndex = k;
						break;
					}
				}
				if (foundCountryIndex != -1)
				{
					countriesCommentsNum[foundCountryIndex]++;
				}
				else
				{
					char(*newCountriesNames)[COUNTRY_SIZE] =
						realloc(countriesNames, (countriesNum + 1) * sizeof(countriesNames[0]));
					if (newCountriesNames == NULL) { // Check if allocation work
						printf("Cannot allocate this memory\n");
						return 0;
					}
					countriesNames = newCountriesNames;
					strcpy(countriesNames[countriesNum], moviesArray[i].p2list[j].country);

					int* newCountriesCommentsNum = realloc(countriesCommentsNum,
						(countriesNum + 1) * sizeof(int));
					if (newCountriesCommentsNum == NULL) { // Check if allocation work
						printf("Cannot allocate this memory\n");
						return 0;
					}

					countriesCommentsNum = newCountriesCommentsNum;
					countriesCommentsNum[countriesNum] = 1;
					countriesNum++;
				}
			}
		}
	}

	// Find max comments num
	int maxCommentsNum = 0;
	for (int i = 0; i < countriesNum; i++)
	{
		if (countriesCommentsNum[i] > maxCommentsNum)
		{
			maxCommentsNum = countriesCommentsNum[i];
		}
	}

	for (int i = 0; i < countriesNum; i++)
	{
		if (countriesCommentsNum[i] == maxCommentsNum)
		{
			printf("%s\n", countriesNames[i]);
		}
	}

	free(countriesNames);
	free(countriesCommentsNum);
}

int recommendMovie(const movie* moviesArray, const int moviesArraySize, const int votesAvg)
{
	FILE* RecommendMovieFile = fopen(RECOMMENDED_FILE, "w");
	if (RecommendMovieFile == NULL) { // checking if the file type is NULL, meaning not opening
		printf("File not opened\n");
		exit(1);
	}

	for (int i = 0; i < moviesArraySize; i++) // movies
	{
		int movieVotesSum = 0;
		for (int j = 0; j < moviesArray[i].Nvotes; j++)
		{
			movieVotesSum += moviesArray[i].p2list[j].value;
		}
		double movieVotesAvg = (double)movieVotesSum / moviesArray[i].Nvotes;
		if (movieVotesAvg > votesAvg)
		{
			fprintf(RecommendMovieFile, "%s,%s\n", moviesArray[i].p2name, moviesArray[i].p2genre);
		}
	}
	fclose(RecommendMovieFile);
}

//Helper function to get genre from letters  
int IsGenreNameValid(const char* genreName)
{
	for (int i = 0; i < strlen(genreName); i++)
	{
		if (!isalpha(genreName[i]))
		{
			printf("movie genre %s isn't only letters\n", genreName);
			return 0;
		}
	}
	return 1;
}

/*This function delete vote with min value. First we found minVote , allocate memory for all votes
and create new array of votes without the minVote in there, in this way we delete the minVotes (if there are many)*/
int deleteWorst(movie* const moviesArray, const int moviesArraySize, const char* genreName)
{
	int movieOfGenreExists = 0, minVote = 10; // Votes between 1-10
	for (int i = 0; i < moviesArraySize; i++)
	{
		if (strcmp(moviesArray[i].p2genre, genreName) == 0) // Check if userGenre equal to genre in the array 
		{
			movieOfGenreExists = 1; //Flag for genre exists
			//Loop on the Nvotes and check if one of them have value that smaller than minVote
			for (int j = 0; j < moviesArray[i].Nvotes; j++)
			{
				if (moviesArray[i].p2list[j].value < minVote)
				{
					minVote = moviesArray[i].p2list[j].value;
				}
			}
		}
	}

	for (int i = 0; i < moviesArraySize; i++)
	{
		if (strcmp(moviesArray[i].p2genre, genreName) == 0 && moviesArray[i].Nvotes > 0) //If Nvotes is 0 we done need to check
		{

			vote* votes2Keep = malloc(moviesArray[i].Nvotes * sizeof(vote)); //Allocate memory in size of Nvotes
			if (votes2Keep == NULL) { // Check if allocation work
				printf("Cannot allocate this memory\n");
				return 0;
			}
			int votes2KeepInd = 0;
			for (int j = 0; j < moviesArray[i].Nvotes; j++)
			{
				if (moviesArray[i].p2list[j].value != minVote) //Get all votes that not equal to minVote 
				{
					votes2Keep[votes2KeepInd] = moviesArray[i].p2list[j];
					votes2KeepInd++;
				}
			}
			if (votes2KeepInd > 0)
			{
				votes2Keep = realloc(votes2Keep, votes2KeepInd * sizeof(vote)); //Update the memory for new array
				if (votes2Keep == NULL) { // Check if allocation work
					printf("Cannot allocate this memory\n");
					return 0;
				}
			}
			else
			{
				free(votes2Keep); //If there is no votes in the new array, free memory
				votes2Keep = NULL;
			}
			moviesArray[i].p2list = votes2Keep;
			moviesArray[i].Nvotes = votes2KeepInd;

		}
	}
	if (!movieOfGenreExists) {
		printf("There is no movies of genre %s\n", genreName);
	}
}


//Prints menu with selection
void printMenu(movie** moviesArray, int* moviesArrSize) {

	int userSelection = 0, userValue = 0, movieId = 0, userYear = 0, votesAvg = 0;
	char movieGenre[LENGTH], userCountry[COUNTRY_SIZE];
	char movieName[BUFFER_SIZE] = { 0 };

	printf("Menu\nSelect a function to run the program\n");
	do
	{
		printf("(0) To exit\n");
		printf("(1) Add Movie\n");
		printf("(2) Add Vote\n");
		printf("(3) Print Votes\n");
		printf("(4) Count Genre\n");
		printf("(5) Print Value\n");
		printf("(6) Count Country\n");
		printf("(7) Max By Country\n");
		printf("(8) Recommend Movie\n");
		printf("(9) Delete Worst\n");
		scanf("%d", &userSelection);
		fixScanfBug();
		fseek(stdin, 0, SEEK_END);

		switch (userSelection) {
		case 0:
			printf("Goodbye :)\n");
			break;
		case 1:
			addMovie(moviesArray, moviesArrSize);
			break;
		case 2:
			printf("Enter movie Id\n");
			scanf("%d", &movieId);
			fixScanfBug();
			addVote(*moviesArray, *moviesArrSize, movieId);
			break;

		case 3:
			printf("Enter movie name\n");
			scanf("%[^\n]", movieName);
			fixScanfBug();
			int printVotesRes = printVotes(*moviesArray, *moviesArrSize, movieName);
			if (printVotesRes == -1)
			{
				printf("There is no movie with name %s\n", movieName);
			}
			else if (printVotesRes == 0)
			{
				printf("No votes for movie with name %s\n", movieName);
			}
			else if (printVotesRes == 1)
			{
				printf("There are votes for movie with name %s\n", movieName);
			}
			break;

		case 4:
			printf("Enter movie genre\n");
			scanf("%29s", movieGenre);
			fixScanfBug();

			if (IsGenreNameValid(movieGenre))
				countGenre(*moviesArray, *moviesArrSize, movieGenre);
			break;

		case 5:
			printf("Enter Value\n");
			scanf("%d", &userValue);
			fixScanfBug();
			printf("Enter Country\n");
			scanf("%s", userCountry);
			fixScanfBug();
			printValue(*moviesArray, *moviesArrSize, userValue, userCountry);
			break;

		case 6:
			printf("Enter year\n");
			scanf("%d", &userYear);
			fixScanfBug();
			countCountry(*moviesArray, *moviesArrSize, userYear);
			break;

		case 7:
			maxByCountry(*moviesArray, *moviesArrSize);
			break;

		case 8:
			printf("Enter votes average\n");
			scanf("%d", &votesAvg);
			fixScanfBug();
			recommendMovie(*moviesArray, *moviesArrSize, votesAvg);
			break;

		case 9:
			printf("Enter movie genre\n");
			scanf("%29s", movieGenre);
			fixScanfBug();
			if (IsGenreNameValid(movieGenre))
				deleteWorst(*moviesArray, *moviesArrSize, movieGenre);
			break;

		default:
			printf("Invalid option selected\n");
			break;
		}

	} while (userSelection != 0);

}

//Helper function to free all memory
void freeMemory(movie** moviesArray, const int moviesArrSize)
{
	for (int i = 0; i < moviesArrSize; i++)
	{
		for (int j = 0; j < (*moviesArray)[i].Nvotes; j++)
		{
			free((*moviesArray)[i].p2list[j].p2comment);
		}
		free((*moviesArray)[i].p2list);

		free((*moviesArray)[i].p2name);
		free((*moviesArray)[i].p2genre);
	}
	free((*moviesArray));
	(*moviesArray) = NULL;
}

//Helper function to update movies and votes files with the updated data
void updateFiles(movie* moviesArray, const int moviesArrSize)
{
	FILE* moviesFile = fopen(MOVIES_DATA_FILE, "w");
	if (moviesFile == NULL) { // checking if the file type is NULL, meaning not opening
		printf("File not opened\n");
		exit(1);
	}

	FILE* votesFile = fopen(VOTING_DATA_FILE, "w");
	if (votesFile == NULL) { // checking if the file type is NULL, meaning not opening
		printf("File not opened\n");
		exit(1);
	}

	fprintf(moviesFile, "%s\n", "format:m_id,movie_name,Genre,Lead Studio,Year");
	fprintf(votesFile, "%s\n", "m_id:vote:coutry:comment //- means an empty comment");

	for (int i = 0; i < moviesArrSize; i++)
	{
		// 4,When in Rome,Comedy,Disney,2010
		fprintf(moviesFile, "%d,%s,%s,%s,%d\n",
			moviesArray[i].id,
			moviesArray[i].p2name,
			moviesArray[i].p2genre,
			moviesArray[i].studio,
			moviesArray[i].year);

		for (int j = 0; j < moviesArray[i].Nvotes; j++)
		{
			// 8:3:France:Boring movie
			fprintf(votesFile, "%d:%d:%s:%s\n",
				moviesArray[i].id,
				moviesArray[i].p2list[j].value,
				moviesArray[i].p2list[j].country,
				moviesArray[i].p2list[j].p2comment);
		}

	}

	fclose(moviesFile);
	fclose(votesFile);
}

void main()
{
	int movieArraySize = 0;
	movie* moviesArray = NULL;

	movieArraySize = countLines(MOVIES_DATA_FILE);

	moviesArray = (movie*)malloc(movieArraySize * sizeof(movie));
	if (!moviesArray)
	{
		printf("Error, file not opened\n");
		exit(1);
	}

	FromFile2Movies(MOVIES_DATA_FILE, moviesArray, movieArraySize);

	FromFile2Votes(VOTING_DATA_FILE, moviesArray, movieArraySize);

	printMenu(&moviesArray, &movieArraySize);

	//Save movies array to files
	updateFiles(moviesArray, movieArraySize);

	//Free movies and their votes.
	freeMemory(&moviesArray, movieArraySize);
}