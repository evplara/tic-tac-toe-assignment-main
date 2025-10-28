#include "Application.h"
#include "imgui/imgui.h"
#include "classes/TicTacToe.h"

namespace ClassGame {
        //
        // our global variables
        //
        TicTacToe *game = nullptr;
        bool gameOver = false;
        int gameWinner = -1;

        //
        // game starting point
        // this is called by the main render loop in main.cpp
        //
        void GameStartUp() 
        {
            game = new TicTacToe();
            game->setUpBoard();
        }

        //
        // game render loop
        // this is called by the main render loop in main.cpp
        //
        void RenderGame() 
        {
                ImGui::DockSpaceOverViewport();

                //ImGui::ShowDemoWindow();

                if (!game) return;
                if (!game->getCurrentPlayer()) return;
                
                ImGui::Begin("Settings");
                ImGui::Text("Current Player Number: %d", game->getCurrentPlayer()->playerNumber());
                ImGui::Text("Current Board State: %s", game->stateString().c_str());
                ImGui::Text("Turn: %d", game->getCurrentPlayer() ? game->getCurrentPlayer()->playerNumber() : -1);

                ImGui::Text("Current Board State: %s", game->stateString().c_str());

                static char boardBuf[16] = "000000000"; // 9 chars + '\0'
                ImGui::InputText("Edit Board", boardBuf, IM_ARRAYSIZE(boardBuf));
                if (ImGui::Button("Apply Edited Board")) {
                    game->stopGame();
                    game->setUpBoard();                 
                    game->setStateString(boardBuf);      
                    gameOver = false;
                    gameWinner = -1;
                }
                ImGui::SeparatorText("Mode");
                static bool vsAI = false;
                bool twoPClicked = ImGui::RadioButton("Two Players", !vsAI);
                ImGui::SameLine();
                bool aiClicked   = ImGui::RadioButton("Vs AI (AI = Player 2 / O)", vsAI);

                if (twoPClicked || aiClicked) {
                    vsAI = aiClicked;

                    game->stopGame();
                    game->setUpBoard();                         // (recreates players/holders)

                    game->getPlayerAt(0)->setAIPlayer(false);   // Player 1 (X) = human
                    game->getPlayerAt(1)->setAIPlayer(vsAI);    // Player 2 (O) = AI only when vsAI

                    gameOver = false;
                    gameWinner = -1;
                }
                ImGui::End();

                ImGui::Begin("GameWindow");
                game->drawFrame();

                if (!gameOver && game->gameHasAI() && game->getCurrentPlayer() && game->getCurrentPlayer()->isAIPlayer()) {
                    game->updateAI(); 
}

                ImGui::End();
        }

        //
        // end turn is called by the game code at the end of each turn
        // this is where we check for a winner
        //
        void EndOfTurn() 
        {
            Player *winner = game->checkForWinner();
            if (winner)
            {
                gameOver = true;
                gameWinner = winner->playerNumber();
            }
            if (game->checkForDraw()) {
                gameOver = true;
                gameWinner = -1;
            }
        }
}
