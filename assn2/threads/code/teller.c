#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "teller.h"
#include "account.h"
#include "error.h"
#include "debug.h"
#include "branch.h"

#include "pthread.h"

/*
 * deposit money into an account
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  // First the account, then the branch
  pthread_mutex_lock(&account->lock);
  pthread_mutex_lock(&bank->branches[AccountNum_GetBranchID(accountNum)].branchLock);

  Account_Adjust(bank,account, amount, 1);

  // Then unlock them in order
  pthread_mutex_unlock(&bank->branches[AccountNum_GetBranchID(accountNum)].branchLock);
  pthread_mutex_unlock(&account->lock);

  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  // First the account, then the branch
  pthread_mutex_lock(&account->lock);
  pthread_mutex_lock(&bank->branches[AccountNum_GetBranchID(accountNum)].branchLock);

  if (amount > Account_Balance(account)) {
    pthread_mutex_unlock(&bank->branches[AccountNum_GetBranchID(accountNum)].branchLock);
    pthread_mutex_unlock(&account->lock);
    return ERROR_INSUFFICIENT_FUNDS;
  }

  Account_Adjust(bank,account, -amount, 1);

  // Then unlock them in order
  pthread_mutex_unlock(&bank->branches[AccountNum_GetBranchID(accountNum)].branchLock);
  pthread_mutex_unlock(&account->lock);

  return ERROR_SUCCESS;
}


/*
 * do a tranfer from one account to another account
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));

  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  if (amount > Account_Balance(srcAccount)) {
    return ERROR_INSUFFICIENT_FUNDS;
  }

  /*
   * If we are doing a transfer within the branch, we tell the Account module to
   * not bother updating the branch balance since the net change for the
   * branch is 0.
   */
  int updateBranch = !Account_IsSameBranch(srcAccountNum, dstAccountNum);

  // First lock down both accounts
  pthread_mutex_t *act1, *act2, *bra1, *bra2;
  if (srcAccount->accountNumber < dstAccount->accountNumber) {
    act1 = &srcAccount->lock;
    act2 = &dstAccount->lock;
    bra1 = &bank->branches[AccountNum_GetBranchID(srcAccountNum)].branchLock;
    bra2 = &bank->branches[AccountNum_GetBranchID(dstAccountNum)].branchLock;
  } else {
    act2 = &srcAccount->lock;
    act1 = &dstAccount->lock;    
    bra2 = &bank->branches[AccountNum_GetBranchID(srcAccountNum)].branchLock;
    bra1 = &bank->branches[AccountNum_GetBranchID(dstAccountNum)].branchLock;
  }

  // Then figure out whether we need the branches locked down
  BranchID srcNum = AccountNum_GetBranchID(srcAccount->accountNumber);
  BranchID dstNum = AccountNum_GetBranchID(dstAccount->accountNumber);

  // Only lock one branch and process
  if (srcNum == dstNum) { 

    // If the same account, make sure we don't do weird things with locks
    if (srcAccount->accountNumber == dstAccount->accountNumber) {
      pthread_mutex_lock(act1);
      Account_Adjust(bank, srcAccount, -amount, updateBranch);
      Account_Adjust(bank, dstAccount, amount, updateBranch);
      pthread_mutex_unlock(act1);
    }

    else {
      pthread_mutex_lock(act1);
      pthread_mutex_lock(act2);
      Account_Adjust(bank, srcAccount, -amount, updateBranch);
      Account_Adjust(bank, dstAccount, amount, updateBranch);
      pthread_mutex_unlock(act2);     
      pthread_mutex_unlock(act1);     
    }

  }

  // Otherwise process both branches
  else {

    pthread_mutex_lock(act1);
    pthread_mutex_lock(act2);
    pthread_mutex_lock(bra1);
    pthread_mutex_lock(bra2);

    Account_Adjust(bank, srcAccount, -amount, updateBranch);
    Account_Adjust(bank, dstAccount, amount, updateBranch);

    pthread_mutex_unlock(bra2);
    pthread_mutex_unlock(bra1);
    pthread_mutex_unlock(act2);
    pthread_mutex_unlock(act1);

  }

  return ERROR_SUCCESS;
}
