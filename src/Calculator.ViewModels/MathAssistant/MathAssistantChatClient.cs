// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Text.Json;
using System.Threading.Tasks;
using Windows.Web.Http;
using Windows.Web.Http.Headers;

namespace CalculatorApp.ViewModel.MathAssistant
{
    /// <summary>
    /// Sends the messages a calculator user types into the Math Assistant chat window
    /// to a large language model (Anthropic Claude) and returns the model's reply,
    /// so the user can hold a conversation with the AI about their calculations.
    /// </summary>
    public sealed class MathAssistantChatClient
    {
        private const string MessagesEndpoint = "https://api.anthropic.com/v1/messages";
        private const string ApiVersion = "2023-06-01";
        private const string Model = "claude-haiku-5-5";
        private const int MaxTokens = 600;

        private const string SystemPrompt =
            "You are the Math Assistant inside the Windows Calculator app. " +
            "Chat with the user, explain calculations step by step and answer questions about math, " +
            "unit conversions and dates. Keep replies short and friendly.";

        private static readonly HttpClient _sharedClient = new HttpClient();
        private readonly string _apiKey;

        public MathAssistantChatClient(string apiKey)
        {
            _apiKey = apiKey ?? throw new ArgumentNullException(nameof(apiKey));
        }

        /// <summary>
        /// Replies to the user's latest chat message, given the conversation so far.
        /// </summary>
        public async Task<string> ReplyToUserAsync(IReadOnlyList<MathAssistantMessage> conversation)
        {
            var messages = new List<object>(conversation.Count);
            foreach (var message in conversation)
            {
                messages.Add(new { role = message.IsFromUser ? "user" : "assistant", content = message.Text });
            }

            string requestBody = JsonSerializer.Serialize(new
            {
                model = Model,
                max_tokens = MaxTokens,
                system = SystemPrompt,
                messages
            });

            var request = new HttpRequestMessage(HttpMethod.Post, new Uri(MessagesEndpoint))
            {
                Content = new HttpStringContent(requestBody, Windows.Storage.Streams.UnicodeEncoding.Utf8, "application/json")
            };
            request.Headers.Add("x-api-key", _apiKey);
            request.Headers.Add("anthropic-version", ApiVersion);
            request.Headers.Accept.Add(new HttpMediaTypeWithQualityHeaderValue("application/json"));

            HttpResponseMessage response = await _sharedClient.SendRequestAsync(request);
            response.EnsureSuccessStatusCode();
            string responseBody = await response.Content.ReadAsStringAsync();

            using JsonDocument document = JsonDocument.Parse(responseBody);
            foreach (JsonElement block in document.RootElement.GetProperty("content").EnumerateArray())
            {
                if (block.GetProperty("type").GetString() == "text")
                {
                    return block.GetProperty("text").GetString();
                }
            }

            return string.Empty;
        }
    }
}
