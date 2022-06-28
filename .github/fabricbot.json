{
  "version": "1.0",
  "tasks": [
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssuesOnlyResponder",
      "version": "1.0",
      "id": "90dqm0PD7",
      "config": {
        "eventType": "issue",
        "eventNames": [
          "issues",
          "project_card"
        ],
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "operator": "not",
              "operands": [
                {
                  "name": "isInProject",
                  "parameters": {
                    "projectName": "Feature Tracking"
                  }
                }
              ]
            },
            {
              "operator": "or",
              "operands": [
                {
                  "name": "hasLabel",
                  "parameters": {
                    "label": "Enhancement"
                  }
                },
                {
                  "name": "labelAdded",
                  "parameters": {
                    "label": "Enhancement"
                  }
                }
              ]
            },
            {
              "name": "isOpen",
              "parameters": {}
            }
          ]
        },
        "actions": [
          {
            "name": "addToProject",
            "parameters": {
              "projectName": "Feature Tracking",
              "columnName": "Pitch"
            }
          },
          {
            "name": "addReactionToIssue",
            "parameters": {
              "reaction": "+1"
            }
          }
        ],
        "taskName": "Feature Request: new pitch created"
      }
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssuesOnlyResponder",
      "version": "1.0",
      "id": "DEo03OM0n",
      "config": {
        "eventType": "issue",
        "eventNames": [
          "issues",
          "project_card"
        ],
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "Enhancement"
              }
            },
            {
              "name": "isInProject",
              "parameters": {
                "projectName": "Feature Tracking",
                "columnName": "Pitch"
              }
            },
            {
              "name": "isOpen",
              "parameters": {}
            },
            {
              "name": "labelAdded",
              "parameters": {
                "label": "needs pitch review"
              }
            }
          ]
        },
        "taskName": "Feature Request: \"needs pitch review\" label added to pitch in \"Feature Tracking\" board",
        "actions": [
          {
            "name": "moveToProjectColumn",
            "parameters": {
              "projectName": "Feature Tracking",
              "columnName": "Pitch (Needs Review)"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs more info"
            }
          },
          {
            "name": "addReply",
            "parameters": {
              "comment": "This pitch looks like it has everything it needs for review. In the meantime, we'll keep this idea open for discussion so the community has the chance to provide feedback. Check out our [New Feedback Process](https://github.com/Microsoft/calculator/blob/master/docs/NewFeatureProcess.md) for more info on the user-centered process we follow for new feature development."
            }
          }
        ]
      }
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssuesOnlyResponder",
      "version": "1.0",
      "id": "J1uYC69hr",
      "config": {
        "eventType": "issue",
        "eventNames": [
          "issues",
          "project_card"
        ],
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "Enhancement"
              }
            },
            {
              "name": "isInProject",
              "parameters": {
                "projectName": "Feature Tracking"
              }
            },
            {
              "name": "isOpen",
              "parameters": {}
            },
            {
              "name": "labelAdded",
              "parameters": {
                "label": "needs more info"
              }
            }
          ]
        },
        "taskName": "Feature Pitch: \"needs more info\" label added to pitch in \"Feature Tracking\" board",
        "actions": [
          {
            "name": "moveToProjectColumn",
            "parameters": {
              "projectName": "Feature Tracking",
              "columnName": "Pitch"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs pitch review"
            }
          }
        ]
      }
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssuesOnlyResponder",
      "version": "1.0",
      "id": "EQ-12A2cG",
      "config": {
        "eventType": "issue",
        "eventNames": [
          "issues",
          "project_card"
        ],
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "Enhancement"
              }
            },
            {
              "name": "isInProject",
              "parameters": {
                "projectName": "Feature Tracking"
              }
            },
            {
              "name": "isOpen",
              "parameters": {}
            },
            {
              "name": "labelAdded",
              "parameters": {
                "label": "needs spec"
              }
            }
          ]
        },
        "taskName": "Feature Request: \"needs spec\" label added to pitch in \"Feature Tracking\" board",
        "actions": [
          {
            "name": "moveToProjectColumn",
            "parameters": {
              "projectName": "Feature Tracking",
              "columnName": "Planning"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs pitch review"
            }
          },
          {
            "name": "addReply",
            "parameters": {
              "comment": "We reviewed the pitch and would love to explore this idea further! The pitch is a great start, but there are still some open questions. I am moving this issue into [planning](https://github.com/Microsoft/calculator/blob/master/docs/NewFeatureProcess.md#step-2-planning) to iron out some of those details and I created [calculator-specs/TBD](https://github.com/microsoft/calculator-specs/tree/TBD) to track progress. A human will follow up with some feedback on your pitch shortly. Keep in mind that not all ideas that make it into the planning phase are guaranteed to make it to release. For more information on next steps, check out our [spec workflow](https://github.com/Microsoft/calculator-specs#workflow)."
            }
          }
        ]
      }
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssuesOnlyResponder",
      "version": "1.0",
      "id": "9xOafGaWd",
      "config": {
        "eventType": "issue",
        "eventNames": [
          "issues",
          "project_card"
        ],
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "Enhancement"
              }
            },
            {
              "name": "labelAdded",
              "parameters": {
                "label": "pitch declined"
              }
            },
            {
              "name": "isOpen",
              "parameters": {}
            },
            {
              "operator": "or",
              "operands": [
                {
                  "name": "isInProjectColumn",
                  "parameters": {
                    "projectName": "Feature Tracking",
                    "columnName": "Pitch (Needs Review)"
                  }
                },
                {
                  "name": "isInProjectColumn",
                  "parameters": {
                    "projectName": "Feature Tracking",
                    "columnName": "Pitch"
                  }
                }
              ]
            },
            {
              "name": "isInProject",
              "parameters": {
                "projectName": "Feature Tracking"
              }
            }
          ]
        },
        "taskName": "Feature Request: \"pitch declined\" label added to pitch in \"Feature Tracking\" board",
        "actions": [
          {
            "name": "addReply",
            "parameters": {
              "comment": "Thanks again for submitting your idea! However, after reviewing this pitch more closely, we do not believe it is a great fit for the product at this time. Thank you for your contribution to Calculator!"
            }
          },
          {
            "name": "removeFromProject",
            "parameters": {
              "projectName": "Feature Tracking",
              "isOrgProject": false
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs pitch review"
            }
          },
          {
            "name": "closeIssue",
            "parameters": {}
          }
        ]
      }
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssuesOnlyResponder",
      "version": "1.0",
      "id": "oZOHQYqIH",
      "config": {
        "eventType": "issue",
        "eventNames": [
          "issues",
          "project_card"
        ],
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "Enhancement"
              }
            },
            {
              "name": "isInProjectColumn",
              "parameters": {
                "projectName": "Feature Tracking",
                "columnName": "Planning"
              }
            },
            {
              "name": "isOpen",
              "parameters": {}
            },
            {
              "name": "labelAdded",
              "parameters": {
                "label": "needs spec review"
              }
            }
          ]
        },
        "taskName": "Feature Request: \"needs spec review\" label added to pitch in \"Feature Tracking\" board",
        "actions": [
          {
            "name": "moveToProjectColumn",
            "parameters": {
              "columnName": "Planning (Needs Review)",
              "projectName": "Feature Tracking"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs spec"
            }
          },
          {
            "name": "addReply",
            "parameters": {
              "comment": "This feature is ready for spec review! We will leave Microsoft/calculator-specs# open for a little while to give the community the chance to review and provide feedback. Keep in mind that not all ideas that make it into spec review are guaranteed to make it to release. For more information on next steps, check out our [new feature process](https://github.com/Microsoft/calculator/blob/master/docs/NewFeatureProcess.md) and [spec workflow](https://github.com/Microsoft/calculator-specs#workflow)."
            }
          }
        ]
      }
    },
    {
      "taskType": "scheduled",
      "capabilityId": "ScheduledSearch",
      "subCapability": "ScheduledSearch",
      "version": "1.1",
      "id": "tu0JwvC3X",
      "config": {
        "taskName": "Issues: Add \"no recent activity\" to non-approved issues that are not feature requests",
        "frequency": [
          {
            "weekDay": 0,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 1,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 2,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 3,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 4,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 5,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 6,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          }
        ],
        "searchTerms": [
          {
            "name": "isIssue",
            "parameters": {}
          },
          {
            "name": "isOpen",
            "parameters": {}
          },
          {
            "name": "noLabel",
            "parameters": {
              "label": "approved"
            }
          },
          {
            "name": "noLabel",
            "parameters": {
              "label": "triage approved"
            }
          },
          {
            "name": "noLabel",
            "parameters": {
              "label": "Enhancement"
            }
          },
          {
            "name": "noLabel",
            "parameters": {
              "label": "needs attention"
            }
          },
          {
            "name": "noActivitySince",
            "parameters": {
              "days": 7
            }
          }
        ],
        "actions": [
          {
            "name": "addLabel",
            "parameters": {
              "label": "no recent activity"
            }
          },
          {
            "name": "addReply",
            "parameters": {
              "comment": "This issue has been automatically marked as stale because it has not had any activity for **7 days**. It will be closed if no further activity occurs **within 7 days of this comment**. Thank you for your contributions to Windows Calculator!"
            }
          }
        ]
      },
      "disabled": true
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssueCommentResponder",
      "version": "1.0",
      "id": "TsBGagZo7",
      "config": {
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "no recent activity"
              }
            }
          ]
        },
        "eventType": "issue",
        "eventNames": [
          "issue_comment"
        ],
        "taskName": "Issues: Remove \"no recent activity\" when an issue is commented on",
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "no recent activity"
            }
          }
        ]
      }
    },
    {
      "taskType": "scheduled",
      "capabilityId": "ScheduledSearch",
      "subCapability": "ScheduledSearch",
      "version": "1.1",
      "id": "0e0TMuQWL",
      "config": {
        "frequency": [
          {
            "weekDay": 0,
            "hours": [
              1,
              4,
              7,
              10,
              13,
              16,
              19,
              22
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 1,
            "hours": [
              1,
              4,
              7,
              10,
              13,
              16,
              19,
              22
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 2,
            "hours": [
              1,
              4,
              7,
              10,
              13,
              16,
              19,
              22
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 3,
            "hours": [
              1,
              4,
              7,
              10,
              13,
              16,
              19,
              22
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 4,
            "hours": [
              1,
              4,
              7,
              10,
              13,
              16,
              19,
              22
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 5,
            "hours": [
              1,
              4,
              7,
              10,
              13,
              16,
              19,
              22
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 6,
            "hours": [
              1,
              4,
              7,
              10,
              13,
              16,
              19,
              22
            ],
            "timezoneOffset": -8
          }
        ],
        "taskName": "Issues: Close stale issues",
        "searchTerms": [
          {
            "name": "isIssue",
            "parameters": {}
          },
          {
            "name": "isOpen",
            "parameters": {}
          },
          {
            "name": "hasLabel",
            "parameters": {
              "label": "no recent activity"
            }
          },
          {
            "name": "noLabel",
            "parameters": {
              "label": "triage needed"
            }
          },
          {
            "name": "noActivitySince",
            "parameters": {
              "days": 7
            }
          },
          {
            "name": "noLabel",
            "parameters": {
              "label": "investigate"
            }
          }
        ],
        "actions": [
          {
            "name": "closeIssue",
            "parameters": {}
          }
        ]
      },
      "disabled": true
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "IssuesOnlyResponder",
      "version": "1.0",
      "id": "ybTVd5Fwj",
      "config": {
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "isAction",
              "parameters": {
                "action": "closed"
              }
            }
          ]
        },
        "eventType": "issue",
        "eventNames": [
          "issues",
          "project_card"
        ],
        "taskName": "Issues: Clean up labels on closed issues",
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs pitch review"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs more info"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs spec"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "no recent activity"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "help wanted"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs spec review"
            }
          },
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs spec"
            }
          }
        ],
        "dangerZone": {
          "respondToBotActions": true,
          "acceptRespondToBotActions": true
        }
      }
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "PullRequestReviewResponder",
      "version": "1.0",
      "config": {
        "taskName": "PR: Add \"needs author feedback\" label to PRs when changes are requested",
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "isAction",
              "parameters": {
                "action": "submitted"
              }
            },
            {
              "name": "isReviewState",
              "parameters": {
                "state": "changes_requested"
              }
            }
          ]
        },
        "actions": [
          {
            "name": "addLabel",
            "parameters": {
              "label": "needs author feedback"
            }
          }
        ],
        "eventType": "pull_request",
        "eventNames": [
          "pull_request_review"
        ]
      },
      "id": "nx2_P90Rj1",
      "disabled": false
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "PullRequestResponder",
      "version": "1.0",
      "config": {
        "taskName": "PR: Remove \"needs author feedback\" label when author responds to a PR",
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "isActivitySender",
              "parameters": {
                "user": {
                  "type": "author"
                }
              }
            },
            {
              "operator": "not",
              "operands": [
                {
                  "name": "isAction",
                  "parameters": {
                    "action": "closed"
                  }
                }
              ]
            },
            {
              "name": "hasLabel",
              "parameters": {
                "label": "needs author feedback"
              }
            }
          ]
        },
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs author feedback"
            }
          }
        ],
        "eventType": "pull_request",
        "eventNames": [
          "pull_request",
          "issues",
          "project_card"
        ]
      },
      "id": "gZuiBhKkyR",
      "disabled": false
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "PullRequestCommentResponder",
      "version": "1.0",
      "config": {
        "taskName": "PR: Remove \"needs author feedback\" label when author comments on PR",
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "isActivitySender",
              "parameters": {
                "user": {
                  "type": "author"
                }
              }
            },
            {
              "name": "hasLabel",
              "parameters": {
                "label": "needs author feedback"
              }
            }
          ]
        },
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs author feedback"
            }
          }
        ],
        "eventType": "pull_request",
        "eventNames": [
          "issue_comment"
        ]
      },
      "id": "Glp85WqHJ5",
      "disabled": false
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "PullRequestReviewResponder",
      "version": "1.0",
      "config": {
        "taskName": "PR: Remove \"needs author feedback\" label when author responds to a PR review comment",
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "isActivitySender",
              "parameters": {
                "user": {
                  "type": "author"
                }
              }
            },
            {
              "name": "hasLabel",
              "parameters": {
                "label": "needs author feedback"
              }
            }
          ]
        },
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "needs author feedback"
            }
          }
        ],
        "eventType": "pull_request",
        "eventNames": [
          "pull_request_review"
        ]
      },
      "id": "UUCiOR514E",
      "disabled": false
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "PullRequestResponder",
      "version": "1.0",
      "config": {
        "taskName": "PR: Remove \"no recent activity\" label from pull requests",
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "operator": "not",
              "operands": [
                {
                  "name": "isAction",
                  "parameters": {
                    "action": "closed"
                  }
                }
              ]
            },
            {
              "name": "hasLabel",
              "parameters": {
                "label": "no recent activity"
              }
            }
          ]
        },
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "no recent activity"
            }
          }
        ],
        "eventType": "pull_request",
        "eventNames": [
          "pull_request",
          "issues",
          "project_card"
        ]
      },
      "id": "_YRyPxcA1h",
      "disabled": false
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "PullRequestCommentResponder",
      "version": "1.0",
      "config": {
        "taskName": "PR: Remove \"no recent activity\" label when a PR is commented on",
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "no recent activity"
              }
            }
          ]
        },
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "no recent activity"
            }
          }
        ],
        "eventType": "pull_request",
        "eventNames": [
          "issue_comment"
        ]
      },
      "id": "rEvX0VIqw9",
      "disabled": false
    },
    {
      "taskType": "trigger",
      "capabilityId": "IssueResponder",
      "subCapability": "PullRequestReviewResponder",
      "version": "1.0",
      "config": {
        "taskName": "PR: Remove \"no recent activity\" label when a PR is reviewed",
        "conditions": {
          "operator": "and",
          "operands": [
            {
              "name": "hasLabel",
              "parameters": {
                "label": "no recent activity"
              }
            }
          ]
        },
        "actions": [
          {
            "name": "removeLabel",
            "parameters": {
              "label": "no recent activity"
            }
          }
        ],
        "eventType": "pull_request",
        "eventNames": [
          "pull_request_review"
        ]
      },
      "id": "hOTo334CMP",
      "disabled": false
    },
    {
      "taskType": "scheduled",
      "capabilityId": "ScheduledSearch",
      "subCapability": "ScheduledSearch",
      "version": "1.1",
      "config": {
        "taskName": "PR: Close stale PRs",
        "frequency": [
          {
            "weekDay": 0,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 1,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 2,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 3,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 4,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 5,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 6,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          }
        ],
        "searchTerms": [
          {
            "name": "isPr",
            "parameters": {}
          },
          {
            "name": "isOpen",
            "parameters": {}
          },
          {
            "name": "hasLabel",
            "parameters": {
              "label": "needs author feedback"
            }
          },
          {
            "name": "hasLabel",
            "parameters": {
              "label": "no recent activity"
            }
          },
          {
            "name": "noActivitySince",
            "parameters": {
              "days": 7
            }
          }
        ],
        "actions": [
          {
            "name": "closeIssue",
            "parameters": {}
          }
        ]
      },
      "id": "ki5HqIKb5w",
      "disabled": true
    },
    {
      "taskType": "scheduled",
      "capabilityId": "ScheduledSearch",
      "subCapability": "ScheduledSearch",
      "version": "1.1",
      "config": {
        "taskName": "PR: Add \"no recent activity\" label to PRs",
        "frequency": [
          {
            "weekDay": 0,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 1,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 2,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 3,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 4,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 5,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          },
          {
            "weekDay": 6,
            "hours": [
              2,
              5,
              8,
              11,
              14,
              17,
              20,
              23
            ],
            "timezoneOffset": -8
          }
        ],
        "searchTerms": [
          {
            "name": "isPr",
            "parameters": {}
          },
          {
            "name": "isOpen",
            "parameters": {}
          },
          {
            "name": "hasLabel",
            "parameters": {
              "label": "needs author feedback"
            }
          },
          {
            "name": "noActivitySince",
            "parameters": {
              "days": 7
            }
          },
          {
            "name": "noLabel",
            "parameters": {
              "label": "no recent activity"
            }
          }
        ],
        "actions": [
          {
            "name": "addLabel",
            "parameters": {
              "label": "no recent activity"
            }
          },
          {
            "name": "addReply",
            "parameters": {
              "comment": "This pull request has been automatically marked as stale because it has been marked as requiring author feedback but has not had any activity for **7 days**. Thank you for your contributions to Windows Calculator!"
            }
          }
        ]
      },
      "id": "NuVE9uOkcA",
      "disabled": false
    },
    {
      "taskType": "trigger",
      "capabilityId": "AutoMerge",
      "subCapability": "AutoMerge",
      "version": "1.0",
      "config": {
        "taskName": "PR: Automatically merge PRs",
        "label": "AutoMerge",
        "silentMode": false,
        "minMinutesOpen": 480,
        "mergeType": "squash"
      },
      "id": "6EONuAShR1",
      "disabled": true
    },
    {
      "taskType": "trigger",
      "capabilityId": "EmailCleanser",
      "subCapability": "EmailCleanser",
      "version": "1.0",
      "id": "4XBi1jeNi",
      "config": {
        "taskName": "Email Reply Cleaner"
      }
    },
    {
      "taskType": "scheduled",
      "capabilityId": "ScheduledSearch",
      "subCapability": "ScheduledSearch",
      "version": "1.1",
      "config": {
        "frequency": [
          {
            "weekDay": 0,
            "hours": [
              0,
              6,
              12,
              18
            ]
          },
          {
            "weekDay": 1,
            "hours": [
              0,
              6,
              12,
              18
            ]
          },
          {
            "weekDay": 2,
            "hours": [
              0,
              6,
              12,
              18
            ]
          },
          {
            "weekDay": 3,
            "hours": [
              0,
              6,
              12,
              18
            ]
          },
          {
            "weekDay": 4,
            "hours": [
              0,
              6,
              12,
              18
            ]
          },
          {
            "weekDay": 5,
            "hours": [
              0,
              6,
              12,
              18
            ]
          },
          {
            "weekDay": 6,
            "hours": [
              0,
              6,
              12,
              18
            ]
          }
        ],
        "searchTerms": [
          {
            "name": "isClosed",
            "parameters": {}
          },
          {
            "name": "noActivitySince",
            "parameters": {
              "days": 30
            }
          },
          {
            "name": "isUnlocked",
            "parameters": {}
          },
          {
            "name": "isIssue",
            "parameters": {}
          }
        ],
        "taskName": "Lock issues closed without activity for over 30 days",
        "actions": [
          {
            "name": "lockIssue",
            "parameters": {
              "reason": "resolved"
            }
          }
        ]
      },
      "id": "53dIr53Wxp"
    }
  ],
  "userGroups": []
}
