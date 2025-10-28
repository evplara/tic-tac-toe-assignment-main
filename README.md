Negamax AI Explanation:

On the AI’s turn, updateAI() runs a Negamax search—a streamlined form of Minimax for zero-sum games. Instead of tracking separate “max” and “min” players, Negamax always evaluates from the current player’s point of view, so a child position’s value is simply the negative of the opponent’s best reply.

Flow:

Generate legal moves (all empty squares).

Simulate each move and call negamax(nextState).

Base cases return fixed scores:

AI win = +1

Draw = 0

AI loss = -1

Propagate values upward with the Negamax rule:
score(move) = -negamax(stateAfterMove)

Pick the move with the highest score and play it on the real board.

Advance the game by calling endTurn() so control returns to the human.

This guarantees optimal play in Tic-Tac-Toe (so the AI never loses and will force a draw if you play perfectly).
