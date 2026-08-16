#include <stdio.h>

#define MAX_GOAL_POSITION 100
#define MAX_TURN_COUNT    100

double dProbability[MAX_TURN_COUNT + 1][MAX_GOAL_POSITION + 1];

double CalculateGoalProbability(int iGoalPositionIndex, int iTurnCount)
{
    int i;
    int j;

    /* 初期化 */
    for (i = 0; i <= iTurnCount; i++)
    {
        for (j = 0; j <= iGoalPositionIndex; j++)
        {
            dProbability[i][j] = 0.0;
        }
    }

    /* スタート地点 */
    dProbability[0][0] = 1.0;

    /* DP */
    for (i = 0; i < iTurnCount; i++)
    {
        for (j = 0; j < iGoalPositionIndex; j++)
        {
            int k;

            for (k = 1; k <= 6; k++)
            {
                int iNextPosition;

                iNextPosition = j + k;

                /* ゴールを超えたらゴールに止まる */
                if (iNextPosition > iGoalPositionIndex)
                {
                    iNextPosition = iGoalPositionIndex;
                }

                dProbability[i + 1][iNextPosition]
                    += dProbability[i][j] / 6.0;
            }
        }
    }

    {
        double dResult = 0.0;

        for (i = 0; i <= iTurnCount; i++)
        {
            dResult += dProbability[i][iGoalPositionIndex];
        }

        return dResult;
    }
}

int main(void)
{
    int iGoalPositionIndex;
    int iTurnCount;

    printf("Goal Position : ");
    scanf("%d", &iGoalPositionIndex);

    printf("Turn Count : ");
    scanf("%d", &iTurnCount);

    printf("\n");

    printf("Goal Probability = %.10f\n",
        CalculateGoalProbability(
            iGoalPositionIndex,
            iTurnCount));

    return 0;
}