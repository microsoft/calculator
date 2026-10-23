// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.ObjectModel;
using System.Linq;
using System.Threading.Tasks;
using CommunityToolkit.Mvvm.ComponentModel;
using CommunityToolkit.Mvvm.Input;

namespace CalculatorApp.ViewModel.MathAssistant
{
    /// <summary>
    /// Backs the Math Assistant chat pane: calculator users type questions in a chat window
    /// and talk directly with an AI chatbot that answers in real time.
    /// </summary>
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class MathAssistantViewModel : ObservableObject
    {
        private readonly MathAssistantChatClient _chatClient;

        [ObservableProperty]
        private string _draftMessage = string.Empty;

        [ObservableProperty]
        private bool _isWaitingForReply;

        public MathAssistantViewModel(MathAssistantChatClient chatClient)
        {
            _chatClient = chatClient ?? throw new ArgumentNullException(nameof(chatClient));
            Conversation = new ObservableCollection<MathAssistantMessage>();
        }

        public ObservableCollection<MathAssistantMessage> Conversation { get; }

        /// <summary>
        /// Sends the user's typed message to the AI chatbot and appends its reply to the chat.
        /// </summary>
        [RelayCommand]
        private async Task SendMessageToAssistantAsync()
        {
            string text = DraftMessage?.Trim();
            if (string.IsNullOrEmpty(text) || IsWaitingForReply)
            {
                return;
            }

            Conversation.Add(new MathAssistantMessage(text, isFromUser: true));
            DraftMessage = string.Empty;
            IsWaitingForReply = true;
            try
            {
                string reply = await _chatClient.ReplyToUserAsync(Conversation.ToList());
                Conversation.Add(new MathAssistantMessage(reply, isFromUser: false));
            }
            finally
            {
                IsWaitingForReply = false;
            }
        }

        [RelayCommand]
        private void ClearConversation()
        {
            Conversation.Clear();
        }
    }
}
