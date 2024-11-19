using System;
using System.IO;
using System.Net;
using System.Web;

namespace CalculatorUITestFramework
{
    public class MockedCurrencyServer : IDisposable
    {
        private HttpListener _listener;

        public MockedCurrencyServer()
        {
            _listener = new HttpListener
            {
                Prefixes =
                {
                    "http://localhost/calctesting/file/",
                }
            };
            _listener.Start();
            _listener.BeginGetContext(OnGetContext, _listener);
        }

        ~MockedCurrencyServer() { DisposeImpl(); }

        public void Dispose()
        {
            DisposeImpl();
            GC.SuppressFinalize(this);
        }

        private void DisposeImpl()
        {
            _listener.Stop();
        }

        private static void OnGetContext(IAsyncResult result)
        {
            var listener = (HttpListener)result.AsyncState;
            var context = listener.EndGetContext(result);
            var request = context.Request;
            var response = context.Response;
            if (request.HttpMethod == "GET" && TryGetServerFile(request.Url, out var content))
            {
                response.Headers.Add("Accept-Ranges", "bytes");
                response.Headers.Add("Content-Type", "application/json");
                var data = System.Text.Encoding.UTF8.GetBytes(content);
                response.StatusCode = 200;
                response.ContentLength64 = data.Length;
                response.OutputStream.Write(data, 0, data.Length);
                response.OutputStream.Close();
                response.Close();
            }
            else
            {
                response.StatusCode = 400;
                response.OutputStream.Close();
            }
            listener.BeginGetContext(OnGetContext, listener);
        }

        private static bool TryGetServerFile(Uri uri, out string content)
        {
            var query = HttpUtility.ParseQueryString(uri.Query);
            var name = query["id"];
            switch (name)
            {
                case "currency converter data":
                    content = File.ReadAllText("MockCurrencyConverterData.json");
                    return true;
                case "currency static data":
                    content = File.ReadAllText("MockCurrencyStaticData.json");
                    return true;
                default:
                    content = null;
                    return false;
            }
        }
    }
}
