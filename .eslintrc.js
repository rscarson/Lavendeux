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
    'strict': 2,

    // Tab indent
    'indent': [2, 'tab'],
    'react/jsx-indent': [2, 'tab'],
    'react/jsx-indent-props': [2, 'tab'],
    'no-tabs': 0,

    // Jest
    'jest/no-standalone-expect': 0,
    'jest/no-conditional-expect': 0,
    
    // Misc
    'arrow-parens': [2, 'as-needed'],
    'object-curly-newline': 0,
    'no-console': 0,
    'no-unused-expressions': 0,
    'react/prop-types': 0,
    'class-methods-use-this': 0,
    'import/prefer-default-export': 0,
    'no-cond-assign': 0,
    'no-restricted-syntax': 0,
    'func-names': 0,
    'no-plusplus': 0,
    'default-case': 0,
    'no-empty': 0,
    'no-param-reassign': 0,
    'no-alert': 0,
    'no-return-assign': 0,
    'react/no-danger': 0,
    'react/jsx-no-bind': 0,
  },
};
