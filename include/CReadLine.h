#ifndef CREADLINE_H
#define CREADLINE_H

#include <CHistory.h>
#include <memory>

struct CReadLineHistoryEntry {
  int         line_num;
  std::string line;

  CReadLineHistoryEntry(int line_num1, const std::string &line1) :
   line_num(line_num1), line(line1) {
  }
};

class CReadLine {
 public:
  using HistoryEntries = std::vector<CReadLineHistoryEntry>;

 public:
  CReadLine();

  virtual ~CReadLine() { }

  void setAutoHistory(bool flag);

  void enableTimeoutHook(int t=100);
  void disableTimeoutHook();

  std::string readLine();

  std::string readLineInterruptable();

  void interrupt();

  void doInterrupt();

  bool isInterruptable() const { return interruptable_; }

  std::string getPrompt() const { return prompt_; }

  void setPrompt(const std::string &prompt);

  void setName(const std::string &name);

  bool eof() { return eof_; }

  virtual bool completeLine(const std::string &, std::string &) { return false; }

  virtual bool showComplete(const std::string &) { return false; }

  virtual bool getPrevCommand(std::string &line);
  virtual bool getNextCommand(std::string &line);

  virtual void timeout() { }

  std::string getBuffer() const;

  void setBuffer(const std::string &str);

  void addHistory(const std::string &line);

  void getHistoryEntries(HistoryEntries &entries);

  void beep();

 private:
  static int rlCompleteLine(int count, int key);
  static int rlShowComplete(int count, int key);
  static int rlPrevCommand(int count, int key);
  static int rlNextCommand(int count, int key);
  static int rlEventHook();
  static int rlInputAvailableHook();

 private:
  CReadLine(const CReadLine &rhs);
  CReadLine &operator=(const CReadLine &rhs);

 private:
  typedef std::unique_ptr<CHistory> CHistoryP;

  static CReadLine *current_;

  std::string prompt_;
  std::string name_;
  bool        eof_         { false };
  CHistoryP   history_;
  bool        autoHistory_ { false };
  bool        interruptable_ { false };
};

#endif
