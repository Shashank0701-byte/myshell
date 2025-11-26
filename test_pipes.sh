#!/bin/bash
# Test script for multiple pipe functionality

echo "==================================="
echo "Testing Multiple Pipe Support"
echo "==================================="
echo ""

# Create test file
echo "Creating test file..."
cat > test_data.txt << EOF
apple
banana
cherry
apple
date
banana
elderberry
fig
apple
grape
EOF

echo "✓ Test file created"
echo ""

echo "Test 1: Single pipe (sort | uniq)"
echo "-----------------------------------"
cat test_data.txt | sort | uniq
echo ""

echo "Test 2: Double pipe (sort | uniq | wc -l)"
echo "-----------------------------------"
cat test_data.txt | sort | uniq | wc -l
echo "Expected: 7 unique fruits"
echo ""

echo "Test 3: Triple pipe (grep 'a' | sort | uniq | head -3)"
echo "-----------------------------------"
cat test_data.txt | grep 'a' | sort | uniq | head -3
echo ""

echo "Test 4: With input redirection"
echo "-----------------------------------"
sort < test_data.txt | uniq | wc -l
echo ""

echo "Test 5: With output redirection"
echo "-----------------------------------"
cat test_data.txt | sort | uniq > sorted_output.txt
echo "✓ Output written to sorted_output.txt"
cat sorted_output.txt
echo ""

# Cleanup
rm -f test_data.txt sorted_output.txt

echo "==================================="
echo "All tests completed!"
echo "==================================="
