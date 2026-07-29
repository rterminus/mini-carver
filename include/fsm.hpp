#ifndef FSM_HPP
#define FSM_HPP

#include <cstdint>

/*!
 * @brief Defines the states the parser can assume.
 */
enum state_e : std::uint8_t { CODE = 0, BLOCK_COMMENT, BLOCK_DOXYGEN, IN_STRING, RAW_STRING };

/*!
 * @brief Encapsulated state machine for parsing files.
 */
class FSM {
private:
  state_e m_state{ CODE };  //!< state of the machine.

public:
  /*!
   * @brief Gets current state of the machine.
   *
   * @return Current state of the machine.
   */
  state_e get_state() const { return m_state; }
  /*!
   * @brief Sets current state of the machine.
   *
   * @param new_state State to replace current state with.
   */
  void set_state(state_e new_state) { m_state = new_state; }
};

#endif
