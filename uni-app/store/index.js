import useUserStore from "./user";
import useConversation from "./conversation";
import useContacts from "./contacts";
import useMsgStore from "./msg";

export const initListener = () => {
	const userStore = useUserStore();
	const conversationStore = useConversation();
	const contactsStore = useContacts();
	const msgStore = useMsgStore();
	userStore.initListener();
	conversationStore.initListener();
	contactsStore.initListener();
	msgStore.initListener();
};

export const initData = () => {
	const userStore = useUserStore();
	const conversationStore = useConversation();
	const contactsStore = useContacts();
	const msgStore = useMsgStore();

	userStore.initStoreData();
	userStore.getSelfUserInfo();
	conversationStore.initStoreData();
	contactsStore.initStoreData();
	msgStore.initStoreData();

	if (!conversationStore.syncServer) {
		initListData();
	}
};

export const initListData = () => {
	const userStore = useUserStore();
	const conversationStore = useConversation();
	const contactsStore = useContacts();
	const msgStore = useMsgStore();

	conversationStore.getConversationListSplit(true);
	conversationStore.getTotalUnreadMsgCount();

	contactsStore.getGrpList();
	contactsStore.getFriendList();
	// contactsStore.getGrpReqListAsRecipient();
	contactsStore.getFriendReqListAsRecipient();
};