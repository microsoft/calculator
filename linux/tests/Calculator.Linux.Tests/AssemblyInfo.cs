// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// The native CalcManager engine initializes shared globals once per load
// (CCalcEngine::InitialOneTimeOnlySetup) and is single-threaded by design
// (upstream drives it from the UI thread only). Parallel xUnit collections
// would construct concurrent engine instances and crash testhost, so all
// tests in this assembly run serially.
using Xunit;

[assembly: CollectionBehavior(DisableTestParallelization = true)]
