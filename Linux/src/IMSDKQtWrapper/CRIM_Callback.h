#ifndef CRIMCALLBACK_QT_H
#define CRIMCALLBACK_QT_H
#include "CRIMCallbacker.h"

class CRIMCallbacker_Qt : public QObject, public CRIMCallbacker
{
	Q_OBJECT

public://主调返回
	void loginSuccess(const char *data, const char *cookie) override;
	void loginFailure(int errCode, const char *errMsg, const char *cookie) override;
	void logoutSuccess(const char *data, const char *cookie) override;
	void logoutFailure(int errCode, const char *errMsg, const char *cookie) override;
	void uploadFileSuccess(const char *data, const char *cookie) override;
	void uploadFileFailure(int errCode, const char *errMsg, const char *cookie) override;
	void uploadFileProgress(int64_t totalBytes, int64_t currentBytes, int64_t saveBytes, const char *cookie) override;
	void sendMsgSuccess(const char *data, const char *cookie) override;
	void sendMsgFailure(int errCode, const char *errMsg, const char *cookie) override;
	void sendMsgProgress(int progress, const char *cookie) override;
	void revokeMsgSuccess(const char *data, const char *cookie) override;
	void revokeMsgFailure(int errCode, const char *errMsg, const char *cookie) override;
	void markMsgAsReadByMsgIDSuccess(const char *data, const char *cookie) override;
	void markMsgAsReadByMsgIDFailure(int errCode, const char *errMsg, const char *cookie) override;
	void typingStatusUpdateSuccess(const char *data, const char *cookie) override;
	void typingStatusUpdateFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getAdvancedHistoryMsgListSuccess(const char *data, const char *cookie) override;
	void getAdvancedHistoryMsgListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getAdvancedHistoryMsgListReverseSuccess(const char *data, const char *cookie) override;
	void getAdvancedHistoryMsgListReverseFailure(int errCode, const char *errMsg, const char *cookie) override;
	void deleteMsgSuccess(const char *data, const char *cookie) override;
	void deleteMsgFailure(int errCode, const char *errMsg, const char *cookie) override;
	void deleteMsgFromLocalSuccess(const char *data, const char *cookie) override;
	void deleteMsgFromLocalFailure(int errCode, const char *errMsg, const char *cookie) override;
	void deleteAllMsgSuccess(const char *data, const char *cookie) override;
	void deleteAllMsgFailure(int errCode, const char *errMsg, const char *cookie) override;
	void deleteAllMsgFromLocalSuccess(const char *data, const char *cookie) override;
	void deleteAllMsgFromLocalFailure(int errCode, const char *errMsg, const char *cookie) override;
	void insertSingleMsgToLocalSuccess(const char *data, const char *cookie) override;
	void insertSingleMsgToLocalFailure(int errCode, const char *errMsg, const char *cookie) override;
	void insertGrpMsgToLocalSuccess(const char *data, const char *cookie) override;
	void insertGrpMsgToLocalFailure(int errCode, const char *errMsg, const char *cookie) override;
	void findMsgListSuccess(const char *data, const char *cookie) override;
	void findMsgListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void searchLocalMsgsSuccess(const char *data, const char *cookie) override;
	void searchLocalMsgsFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setMsgLocalExSuccess(const char *data, const char *cookie) override { CRIM_UNUSED(data); CRIM_UNUSED(cookie); }
	void setMsgLocalExFailure(int errCode, const char *errMsg, const char *cookie) override { CRIM_UNUSED(errCode); CRIM_UNUSED(errMsg); CRIM_UNUSED(cookie); }
	/**********************************会话**********************************/
	void getAllConversationListSuccess(const char *data, const char *cookie) override;
	void getAllConversationListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getConversationListSplitSuccess(const char *data, const char *cookie) override;
	void getConversationListSplitFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getOneConversationSuccess(const char *data, const char *cookie) override;
	void getOneConversationFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getMultipleConversationSuccess(const char *data, const char *cookie) override;
	void getMultipleConversationFailure(int errCode, const char *errMsg, const char *cookie) override;
	void hideConversationSuccess(const char *data, const char *cookie) override;
	void hideConversationFailure(int errCode, const char *errMsg, const char *cookie) override;
	void hideAllConversationsSuccess(const char *data, const char *cookie) override;
	void hideAllConversationsFailure(int errCode, const char *errMsg, const char *cookie) override;
	void clearConversationAndDeleteAllMsgSuccess(const char *data, const char *cookie) override;
	void clearConversationAndDeleteAllMsgFailure(int errCode, const char *errMsg, const char *cookie) override;
	void deleteConversationAndDeleteAllMsgSuccess(const char *data, const char *cookie) override;
	void deleteConversationAndDeleteAllMsgFailure(int errCode, const char *errMsg, const char *cookie) override;
	void pinConversationSuccess(const char *data, const char *cookie) override;
	void pinConversationFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setConversationDraftSuccess(const char *data, const char *cookie) override;
	void setConversationDraftFailure(int errCode, const char *errMsg, const char *cookie) override;
	void resetConversationGrpAtTypeSuccess(const char *data, const char *cookie) override;
	void resetConversationGrpAtTypeFailure(int errCode, const char *errMsg, const char *cookie) override;
	void markConversationMsgAsReadSuccess(const char *data, const char *cookie) override;
	void markConversationMsgAsReadFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getTotalUnreadMsgCountSuccess(const char *data, const char *cookie) override;
	void getTotalUnreadMsgCountFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setConversationPrivateChatSuccess(const char *data, const char *cookie) override;
	void setConversationPrivateChatFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setConversationBurnDurationSuccess(const char *data, const char *cookie) override;
	void setConversationBurnDurationFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setConversationRecvMsgOptSuccess(const char *data, const char *cookie) override;
	void setConversationRecvMsgOptFailure(int errCode, const char *errMsg, const char *cookie) override;
	/**********************************用户**********************************/
	void setSelfInfoSuccess(const char *data, const char *cookie) override;
	void setSelfInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getSelfInfoSuccess(const char *data, const char *cookie) override;
	void getSelfInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getUsersInfoSuccess(const char *data, const char *cookie) override;
	void getUsersInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getUserStatusSuccess(const char *data, const char *cookie) override;
	void getUserStatusFailure(int errCode, const char *errMsg, const char *cookie) override;
	void subscribeUsersStatusSuccess(const char *data, const char *cookie) override;
	void subscribeUsersStatusFailure(int errCode, const char *errMsg, const char *cookie) override;
	void unsubscribeUsersStatusSuccess(const char *data, const char *cookie) override;
	void unsubscribeUsersStatusFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getSubscribeUsersStatusSuccess(const char *data, const char *cookie) override;
	void getSubscribeUsersStatusFailure(int errCode, const char *errMsg, const char *cookie) override;
	/**********************************好友**********************************/
	void getFriendListSuccess(const char *data, const char *cookie) override;
	void getFriendListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getFriendListPageSuccess(const char *data, const char *cookie) override;
	void getFriendListPageFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getSpecifiedFriendsInfoSuccess(const char *data, const char *cookie) override;
	void getSpecifiedFriendsInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void addFriendSuccess(const char *data, const char *cookie) override;
	void addFriendFailure(int errCode, const char *errMsg, const char *cookie) override;
	void deleteFriendSuccess(const char *data, const char *cookie) override;
	void deleteFriendFailure(int errCode, const char *errMsg, const char *cookie) override;
	void checkFriendSuccess(const char *data, const char *cookie) override;
	void checkFriendFailure(int errCode, const char *errMsg, const char *cookie) override;
	void updateFriendsSuccess(const char *data, const char *cookie) override;
	void updateFriendsFailure(int errCode, const char *errMsg, const char *cookie) override;
	void searchFriendsSuccess(const char *data, const char *cookie) override;
	void searchFriendsFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getFriendReqListAsApplicantSuccess(const char *data, const char *cookie) override;
	void getFriendReqListAsApplicantFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getFriendReqListAsRecipientSuccess(const char *data, const char *cookie) override;
	void getFriendReqListAsRecipientFailure(int errCode, const char *errMsg, const char *cookie) override;
	void acceptFriendReqSuccess(const char *data, const char *cookie) override;
	void acceptFriendReqFailure(int errCode, const char *errMsg, const char *cookie) override;
	void refuseFriendReqSuccess(const char *data, const char *cookie) override;
	void refuseFriendReqFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getBlackListSuccess(const char *data, const char *cookie) override;
	void getBlackListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void addToBlackListSuccess(const char *data, const char *cookie) override;
	void addToBlackListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void removeFromBlackListSuccess(const char *data, const char *cookie) override;
	void removeFromBlackListFailure(int errCode, const char *errMsg, const char *cookie) override;
	/**********************************群组**********************************/
	void createGrpSuccess(const char *data, const char *cookie) override;
	void createGrpFailure(int errCode, const char *errMsg, const char *cookie) override;
	void joinGrpSuccess(const char *data, const char *cookie) override;
	void joinGrpFailure(int errCode, const char *errMsg, const char *cookie) override;
	void quitGrpSuccess(const char *data, const char *cookie) override;
	void quitGrpFailure(int errCode, const char *errMsg, const char *cookie) override;
	void dismissGrpSuccess(const char *data, const char *cookie) override;
	void dismissGrpFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setGrpInfoSuccess(const char *data, const char *cookie) override;
	void setGrpInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getJoinedGrpListSuccess(const char *data, const char *cookie) override;
	void getJoinedGrpListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getSpecifiedGrpsInfoSuccess(const char *data, const char *cookie) override;
	void getSpecifiedGrpsInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void searchGrpsSuccess(const char *data, const char *cookie) override;
	void searchGrpsFailure(int errCode, const char *errMsg, const char *cookie) override;
	void changeGrpMuteSuccess(const char *data, const char *cookie) override;
	void changeGrpMuteFailure(int errCode, const char *errMsg, const char *cookie) override;
	void changeGrpMemberMuteSuccess(const char *data, const char *cookie) override;
	void changeGrpMemberMuteFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setGrpMemberRoleLevelSuccess(const char *data, const char *cookie) override;
	void setGrpMemberRoleLevelFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setGrpLookMemberInfoSuccess(const char *data, const char *cookie) override;
	void setGrpLookMemberInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setGrpApplyMemberFriendSuccess(const char *data, const char *cookie) override;
	void setGrpApplyMemberFriendFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getGrpMemberListSuccess(const char *data, const char *cookie) override;
	void getGrpMemberListFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getGrpMemberListByJoinTimeFilterSuccess(const char *data, const char *cookie) override;
	void getGrpMemberListByJoinTimeFilterFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setGrpMemberInfoSuccess(const char *data, const char *cookie) override;
	void setGrpMemberInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getSpecifiedGrpMembersInfoSuccess(const char *data, const char *cookie) override;
	void getSpecifiedGrpMembersInfoFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setGrpMemberNicknameSuccess(const char *data, const char *cookie) override;
	void setGrpMemberNicknameFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getGrpMemberOwnerAndAdminSuccess(const char *data, const char *cookie) override;
	void getGrpMemberOwnerAndAdminFailure(int errCode, const char *errMsg, const char *cookie) override;
	void setGrpVerificationSuccess(const char *data, const char *cookie) override;
	void setGrpVerificationFailure(int errCode, const char *errMsg, const char *cookie) override;
	void inviteUserToGrpSuccess(const char *data, const char *cookie) override;
	void inviteUserToGrpFailure(int errCode, const char *errMsg, const char *cookie) override;
	void kickGrpMemberSuccess(const char *data, const char *cookie) override;
	void kickGrpMemberFailure(int errCode, const char *errMsg, const char *cookie) override;
	void transferGrpOwnerSuccess(const char *data, const char *cookie) override;
	void transferGrpOwnerFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getGrpReqListAsRecipientSuccess(const char *data, const char *cookie) override;
	void getGrpReqListAsRecipientFailure(int errCode, const char *errMsg, const char *cookie) override;
	void getGrpReqListAsApplicantSuccess(const char *data, const char *cookie) override;
	void getGrpReqListAsApplicantFailure(int errCode, const char *errMsg, const char *cookie) override;
	void acceptGrpReqSuccess(const char *data, const char *cookie) override;
	void acceptGrpReqFailure(int errCode, const char *errMsg, const char *cookie) override;
	void refuseGrpReqSuccess(const char *data, const char *cookie) override;
	void refuseGrpReqFailure(int errCode, const char *errMsg, const char *cookie) override;
	void isJoinGrpSuccess(const char *data, const char *cookie) override;
	void isJoinGrpFailure(int errCode, const char *errMsg, const char *cookie) override;
	void searchGrpMembersSuccess(const char *data, const char *cookie) override;
	void searchGrpMembersFailure(int errCode, const char *errMsg, const char *cookie) override;

public://回调
	/**********************************连接**********************************/
	void notifyConnecting() override;
	void notifyConnectSuccess() override;
	void notifyConnectFailed(int errCode, const char *errMsg, const char *cookie) override;
	void notifyKickedOffline() override;
	void notifyUserTokenExpired() override;
	void notifyUserTokenInvalid(const char *data) override;
	void notifySyncServerStart(int ret) override;
	void notifySyncServerFinish(int ret) override;
	void notifySyncServerFailed(int ret) override;
	void notifySyncServerProgress(int ret) override;
	/**********************************用户**********************************/
	void notifySelfInfoUpdated(const char *data) override;
	void notifyUserStatusChanged(const char *data) override;
	void notifyUserCommandAdd(const char *data) override;
	void notifyUserCommandDelete(const char *data) override;
	void notifyUserCommandUpdate(const char *data) override;
	/**********************************消息**********************************/
	void notifyRecvNewMsgs(const char *data) override;
	void notifyRecvMsgRevoked(const char *data) override;
	void notifyMsgDeleted(const char *data) override;
	void notifyRecv1v1ReadReceipt(const char *data) override;
	/**********************************好友**********************************/
	void notifyFriendAdded(const char *data) override;
	void notifyFriendDeleted(const char *data) override;
	void notifyFriendInfoChanged(const char *data) override;
	void notifyFriendReqAdded(const char *data) override;
	void notifyFriendReqDeleted(const char *data) override;
	void notifyFriendReqAccepted(const char *data) override;
	void notifyFriendReqRejected(const char *data) override;
	void notifyBlackListAdded(const char *data) override;
	void notifyBlackListDeleted(const char *data) override;
	/**********************************会话**********************************/
	void notifyNewConversation(const char *data) override;
	void notifyConversationChanged(const char *data) override;
	void notifyTotalUnreadMsgCountChanged(int num) override;
	/**********************************群组**********************************/
	void notifyGrpInfoChanged(const char *data) override;
	void notifyJoinedGrpDismissed(const char *data) override;
	void notifyJoinedGrpAdded(const char *data) override;
	void notifyJoinedGrpDeleted(const char *data) override;
	void notifyGrpMemberInfoChanged(const char *data) override;
	void notifyGrpMemberAdded(const char *data) override;
	void notifyGrpMemberDeleted(const char *data) override;
	void notifyGrpReqAdded(const char *data) override;
	void notifyGrpReqDeleted(const char *data) override;
	void notifyGrpReqAccepted(const char *data) override;
	void notifyGrpReqRejected(const char *data) override;

};

#endif//CRIMCALLBACK_QT_H
