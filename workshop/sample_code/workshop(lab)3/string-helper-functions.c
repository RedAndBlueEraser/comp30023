/*
	Sample helper function for Workshop task 3 
	we do not want you to waste a lot of time
	doing string processing.
	
	Note: you do not have to uses these functions.
	
*/	

char **tokenizer(char *str){
  char *cur = str;
  char delimiter = ' ';
  int num_tokens = str_char_count(str, delimiter);
  int cur_num = 0;
  char **str_array = malloc((size_t)sizeof(char*) * (size_t)(num_tokens+2));
  /* should you check malloc ? */ 
  char *prev = str;
  while (*cur) {
    if (!*(cur+1)) {
      cur++;
      *cur = delimiter;
    }
    if (*cur == delimiter) {
      char *len = (char*)(cur-prev);
      char *token =  malloc(sizeof(char) * (size_t)(len+1));
      m_strcpy_slice(token, prev, (int) len);
      str_array[cur_num] = token;
      prev = cur;
      prev++;
      cur_num++;
    }
    cur++;
  }
  str_array[num_tokens+1] = NULL;
  return str_array;
}


void str_array_free(char **str_array) {
  char **cur = str_array;
  while (*cur) {
    free(*cur);
    cur++;
  }
  free(str_array);
}

int str_char_count(char *str, char cmp) {
  int count = 0;
  while (*str != '\0') {
    if (*str++ == cmp) {
      count++;
    }
  }
  return count;
}

int m_strcpy_slice(char *dst, char *src, int n) {
  for (int i = 0; i < n; i++) {
    if ((*dst++ = *src++) == '\0') {
      return -1;
    }
  }
  *dst = '\0';
  return 0;
}

void m_strcpy(char *dst, char *src) {
  while ((*dst++ = *src++) != '\0');
}



int m_strcmp(char *a, char *b) {
  char *q1 = a;
  char *q2 = b;
  while((*q1 == *q2) && *q1) {
    q1++;
    q2++;
  }
  return (*q1 - *q2);
}

int m_strlen(char *str) {
  int i = 0;
  while (*str++) {
    i++;
  }
  return i;
}
