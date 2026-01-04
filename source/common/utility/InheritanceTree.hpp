#pragma once

#include <map>
#include <vector>
#include <string>
#include <deque>

template <typename T>
class InheritanceTree
{
public:
    struct Node
    {
        std::string name;
        T val;
        std::vector<Node*> child;

        Node()
        {
        }

        Node(const std::string& name)
        {
            this->name = name;
        }
    };

private:
    std::map<const std::string, Node> m_nodes;

public:
    InheritanceTree() {}

public:
    Node& _node(const std::string& name)
    {
        typename std::map<const std::string, Node>::iterator it = m_nodes.find(name);
        if (it != m_nodes.end())
        {
            return it->second;
        }
        else
        {
            Node& node = m_nodes[name];
            node = Node(name);
            return node;
        }
    }

    template <class TVisitor>
    void visitBFS(TVisitor& visitor)
    {
        std::deque<Node*> queue;

        queue.push_back(&m_nodes[""]);

        while (!queue.empty())
        {
            Node* currentNode = queue.front();
            queue.pop_front();

            // Skip root node
            if (!currentNode->name.empty())
            {
                visitor(currentNode->name, currentNode->val);
            }

            // Queue child nodes
            for (size_t i = 0; i < currentNode->child.size(); i++)
            {
                queue.push_back(currentNode->child[i]);
            }
        }
    }
};
