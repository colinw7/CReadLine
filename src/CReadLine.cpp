#include <CReadLine.h>
#include <readline/readline.h>
#include <readline/history.h>

CReadLine *CReadLine::current_ = nullptr;

CReadLine::
CReadLine() :
 prompt_("> "), name_("readline")
{
  rl_readline_name = name_.c_str();

  rl_catch_signals = 0;

  using BindFn = int (*)(int, int);

  rl_bind_key('\t'  , static_cast<BindFn>(rlCompleteLine));
  rl_bind_key('\033', static_cast<BindFn>(rlCompleteLine));

  rl_set_key("\\C-d", static_cast<BindFn>(rlShowComplete), rl_get_keymap());

#ifdef OS_WIN
  rl_set_key("\033[0A", static_cast<BindFn>(rlPrevCommand), rl_get_keymap());
  rl_set_key("\033[0B", static_cast<BindFn>(rlNextCommand), rl_get_keymap());
  rl_set_key("\033[0C", rl_named_function("forward-char" ), rl_get_keymap());
  rl_set_key("\033[0D", rl_named_function("backward-char"), rl_get_keymap());
#endif

  rl_set_key("\033[A", static_cast<BindFn>(rlPrevCommand), rl_get_keymap());
  rl_set_key("\033[B", static_cast<BindFn>(rlNextCommand), rl_get_keymap());
  rl_set_key("\033[C", rl_named_function("forward-char" ), rl_get_keymap());
  rl_set_key("\033[D", rl_named_function("backward-char"), rl_get_keymap());

  rl_set_key("\033[OA", static_cast<BindFn>(rlPrevCommand), rl_get_keymap());
  rl_set_key("\033[OB", static_cast<BindFn>(rlNextCommand), rl_get_keymap());
  rl_set_key("\033[OC", rl_named_function("forward-char" ), rl_get_keymap());
  rl_set_key("\033[OD", rl_named_function("backward-char"), rl_get_keymap());
}

void
CReadLine::
setAutoHistory(bool flag)
{
  autoHistory_ = flag;

  if (flag)
    history_ = CHistoryP(new CHistory());
  else
    history_ = nullptr;
}

void
CReadLine::
enableTimeoutHook(int t)
{
  rl_event_hook = rlEventHook;

  rl_set_keyboard_input_timeout(t);
}

void
CReadLine::
disableTimeoutHook()
{
  rl_event_hook = 0;
}

std::string
CReadLine::
readLine()
{
  current_ = this;

  char *p = ::readline(const_cast<char *>(prompt_.c_str()));

  if (! p) {
    eof_ = true;

    return "";
  }

  if (autoHistory_)
    addHistory(p);

  return p;
}

std::string
CReadLine::
readLineInterruptable()
{
  interruptable_ = true;

  std::string line = readLine();

  interruptable_ = false;

  return line;
}

void
CReadLine::
setPrompt(const std::string &prompt)
{
  prompt_ = prompt;
}

void
CReadLine::
setName(const std::string &name)
{
  name_ = name;

  rl_readline_name = name_.c_str();
}

bool
CReadLine::
getPrevCommand(std::string &line)
{
  if (history_)
    return history_->getPrevCommand(line);
  else
    return false;
}

bool
CReadLine::
getNextCommand(std::string &line)
{
  if (history_)
    return history_->getNextCommand(line);
  else
    return false;
}

int
CReadLine::
rlCompleteLine(int, int)
{
  if (rl_point != rl_end)
    return 0;

  std::string line = current_->getBuffer();

  std::string line1;

  if (current_->completeLine(line, line1))
    current_->setBuffer(line + line1);

  return 0;
}

int
CReadLine::
rlShowComplete(int, int)
{
  if (rl_point != rl_end) {
    rl_end = rl_point;

    rl_line_buffer[rl_end] = '\0';
  }
  else {
    std::string line = current_->getBuffer();

    if (current_->showComplete(line))
      rl_forced_update_display();
  }

  return 0;
}

int
CReadLine::
rlPrevCommand(int, int)
{
  std::string line;

  if (current_->getPrevCommand(line))
    current_->setBuffer(line);

  return 0;
}

int
CReadLine::
rlNextCommand(int, int)
{
  std::string line;

  if (! current_->getNextCommand(line))
    line = "";

  current_->setBuffer(line);

  return 0;
}

int
CReadLine::
rlEventHook()
{
  current_->timeout();

  return 1;
}

std::string
CReadLine::
getBuffer() const
{
  return rl_line_buffer;
}

void
CReadLine::
setBuffer(const std::string &buffer)
{
  rl_extend_line_buffer(int(buffer.size() + 1));

  strcpy(rl_line_buffer, buffer.c_str());

  rl_end = int(buffer.size());

  rl_point = rl_end;
}

void
CReadLine::
addHistory(const std::string &line)
{
  if (! history_)
    history_ = CHistoryP(new CHistory);

  history_->addCommand(line);
}

void
CReadLine::
getHistoryEntries(std::vector<CReadLineHistoryEntry> &entries)
{
  if (history_) {
    std::vector<std::string> commands;

    history_->getCommands(commands);

    for (size_t i = 0; i < commands.size(); ++i) {
      CReadLineHistoryEntry entry(int(i), commands[i]);

      entries.push_back(entry);
    }
  }
  else {
    HIST_ENTRY **hist_entries = history_list();

    for (int i = 0; hist_entries && hist_entries[i]; ++i) {
      CReadLineHistoryEntry entry(i + history_base, hist_entries[i]->line);

      entries.push_back(entry);
    }
  }
}

void
CReadLine::
beep()
{
  rl_ding();
}

void
CReadLine::
interrupt()
{
  rl_crlf();

  setBuffer("");

  rl_forced_update_display();
}

void
CReadLine::
doInterrupt()
{
  if (interruptable_)
    rl_done = 1;
}
