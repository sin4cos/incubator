# 哈希
1. 两数之和
```c++
vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> map;
    for (int i = 0; i < nums.size(); ++i) {
        auto expected = target - nums[i];
        if (map.find(expected) != map.end()) {
            return {i, map[expected]};
        }
        map[nums[i]] = i;
    }
    return {};
}
```

49. 字母异位词分组
```c++
vector<vector<string>> groupAnagrams(vector<string>& strs) {
    unordered_map<string, vector<string>> map(strs.size());
    for (const auto& s : strs) {
        string cpy = s;
        sort(cpy.begin(), cpy.end());
        map[cpy].emplace_back(s);
    }
    vector<vector<string>> result;
    for (const auto& m : map) {
        result.emplace_back(std::move(m.second));
    }
    return result;
}
```

128. 最长连续序列
```c++
int longestConsecutive(vector<int>& nums) {
    unordered_set<int> uset;
    for (auto n : nums) {
        uset.emplace(n);
    }
    int longest = 0;
    for (int i = 0; i < nums.size(); ++i) {
        if (!uset.count(nums[i]-1)) {
            int current_num = nums[i];
            int current_len = 1;
            while (uset.count(current_num+1)) {
                current_num++;
                current_len++;
            }
            longest = max(longest, current_len);
        }
    }
    return longest;
}
```

# 双指针
283. 移动零
```c++
void moveZeroes(vector<int>& nums) {
    for (int l = 0, r = 0; r < nums.size(); ++r) {
        if (nums[r] != 0) {
            swap(nums[l++], nums[r]);
        }
    }
}
```

11. 盛最多水的容器
```c++
int maxArea(vector<int>& height) {
    int L = 0, R = height.size() - 1;
    int max_area = 0;
    while (L < R) {
        int lower = min(height[L], height[R]);
        max_area = max(max_area, lower * (R - L));
        if (height[L] < height[R])
            L++;
        else
            R--;
    }
    return max_area;
}
```

15. 三数之和
```c++
vector<vector<int>> threeSum(vector<int>& nums) {
    vector<vector<int>> result;

    sort(nums.begin(), nums.end());

    for (int i = 0; i < nums.size(); ++i) {
        if (nums[i] > 0)  break;
        if (i > 0 && nums[i-1] == nums[i]) continue;

        int l = i + 1, r = nums.size() - 1;
        while (l < r) {
            auto total = nums[i] + nums[l] + nums[r];
            if (total == 0) {
                result.emplace_back(std::move(vector{nums[i], nums[l], nums[r]}));
                while (l < r && nums[l] == nums[l+1]) l++;
                while (l < r && nums[r] == nums[r-1]) r--;
                l++, r--;
            } else if (total > 0) {
                r--;
            } else {
                l++;
            }
        }
    }
    return result;
}
```

42. 接雨水
```c++
int trap(vector<int>& height) { 
    int l = 1, lmax = 0;
    int r = height.size() - 2, rmax = 0;
    int result = 0;
    while (l <= r) {
        if (height[l-1] < height[r+1]) {
            lmax = max(lmax, height[l-1]);
            auto diff = lmax - height[l];
            if (diff > 0)
                result += diff;
            l++;
        } else {
            rmax = max(rmax, height[r+1]);
            auto diff = rmax - height[r];
            if (diff > 0)
                result += diff;
            r--;
        }
    }
    return result;
}
```

# 滑动窗口
3. 无重复字符的最长子串
```c++
int lengthOfLongestSubstring(string s) {
    char table[128] = {0};
    int longest = 0;
    for (int l = 0, r = 0; r < s.size(); ++r) {
        ++table[s[r]];
        while (table[s[r]] > 1) {
            --table[s[l++]];
        }
        longest = max(longest, r - l + 1);
    }
    return longest;
}
```

438. 找到字符串中所有字母异位词
```c++
vector<int> findAnagrams(string s, string p) {
    int ssize = s.size(), psize = p.size();
    if (ssize < psize)
        return {};
    char swindow[26] = {0}, pwindow[26] = {0};
    for (int i = 0; i < psize; ++i) {
        swindow[s[i] - 'a']++;
        pwindow[p[i] - 'a']++;
    }
    vector<int> result;
    if (memcmp(swindow, pwindow, sizeof(swindow)) == 0) {
        result.emplace_back(0);
    }
    for (int i = 0; i < ssize - psize; ++i) {
        --swindow[s[i] - 'a'];
        ++swindow[s[i + psize] - 'a'];
        if (memcmp(swindow, pwindow, sizeof(swindow)) == 0) {
            result.emplace_back(i+1);
        }
    }
    return result;
}
```

# 子串
560. 和为 K 的子数组
```c++
int subarraySum(vector<int>& nums, int k) {
    unordered_map<int, int> presum;     // <前缀和, 当前前缀和的次数>
    presum[0] = 1;
    int sum = 0, count = 0;
    for (int i = 0; i < nums.size(); ++i) {
        sum += nums[i];
        if (presum.find(sum - k) != presum.end()) {
            count += presum[sum - k];
        }
        presum[sum]++;
    }
    return count;
}
```

239. 滑动窗口最大值



# 链表
160. 相交链表
```c++
ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
    if (headA == nullptr || headB == nullptr)
        return nullptr;
    ListNode *l1 = headA, *l2 = headB;
    while (l1 != l2) {
        l1 = l1 == nullptr ? headB : l1->next;
        l2 = l2 == nullptr ? headA : l2->next;
    }
    return l1;
}
```

206. 反转链表
```c++
ListNode* reverseList(ListNode* head) {
    ListNode *new_head = new ListNode(), *curr = head;
    while (curr) {
        ListNode *next = curr->next;
        curr->next = new_head->next;
        new_head->next = curr;
        curr = next;
    }
    ListNode *ans = new_head->next;
    delete new_head;
    return ans;
}
```

234. 回文链表
```c++
bool isPalindrome(ListNode* head) {
    if (head == nullptr || head->next == nullptr)
        return true;
    
    ListNode *slow = head, *fast = head;
    while (fast->next && fast->next->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    ListNode *l1 = reverseList(slow->next);
    ListNode *l2 = head;
    while (l1) {
        if (l1->val != l2->val) {
            return false;
        }
        l1 = l1->next;
        l2 = l2->next;
    }
    return true;
}

ListNode *reverseList(ListNode* head) {
    if (head == nullptr)
        return nullptr;
    ListNode *prev = nullptr, *curr = head, *next = nullptr;
    while (curr) {
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    return prev;
}
```

141. 环形链表
```c++
bool hasCycle(ListNode *head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            return true;
        }
    }
    return false;
}
```

142. 环形链表 II
```c++
ListNode *detectCycle(ListNode *head) {
    ListNode *slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            ListNode *ans = head;
            while (ans != slow) {
                ans = ans->next;
                slow = slow->next;
            }
            return ans;
        }
    }
    return nullptr;
}
```

21. 合并两个有序链表
```c++
ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
    ListNode *head = new ListNode();
    ListNode *l1 = list1, *l2 = list2, *curr = head;
    while (l1 && l2) {
        if (l1->val < l2->val) {
            curr->next = l1;
            l1 = l1->next;
        } else {
            curr->next = l2;
            l2 = l2->next;
        }
        curr = curr->next;
    }
    curr->next = l1 ? l1 : l2;
    return head->next;
}
```

2. 两数相加
```c++
ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode *dummy = new ListNode(0), *curr = dummy;
    int carry = 0;
    while (l1 || l2) {
        int v1 = l1 ? l1->val : 0;
        int v2 = l2 ? l2->val : 0;
        int sum = v1 + v2 + carry;
        carry = sum / 10;
        sum = sum % 10;
        curr->next = new ListNode(sum);
        curr = curr->next;
        if (l1)
            l1 = l1->next;
        if (l2)
            l2 = l2->next;
    }
    if (carry) {
        curr->next = new ListNode(carry);
        curr = curr->next;
    }
    ListNode *ans = dummy->next;
    delete dummy;
    return ans;
}
```

19. 删除链表的倒数第 N 个结点
```c++
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode *dummy = new ListNode(0, head);
    ListNode *slow = dummy, *fast = head;
    while (n-- > 0 && fast)
        fast = fast->next;
    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }
    slow->next = slow->next->next;
    ListNode *ans = dummy->next;
    delete dummy;
    return ans;
}
```

24. 两两交换链表中的节点