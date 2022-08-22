module.exports = {
  env: {
    browser: true,
    es2021: true,
    'jest/globals': true
  },
  extends: [
    'plugin:jest/recommended',
    'plugin:react/recommended',
    'airbnb',
  ],
  overrides: [
  ],
  parserOptions: {
    ecmaVersion: 'latest',
    sourceType: 'module',
  },
  plugins: [
    'react',
  ],
  rules: {
    'react/jsx-filename-extension': [1, { 'extensions': ['.js', '.jsx'] }],

    // Tab indent
    'indent': [2, 'tab'],
    'react/jsx-indent': [2, 'tab'],
    'react/jsx-indent-props': [2, 'tab'],
    'no-tabs': 0,
    
    // Misc
    'arrow-parens': [2, 'as-needed'],
    'object-curly-newline': 0,
    'no-console': 0,
    'no-unused-expressions': 0,
  },
};
