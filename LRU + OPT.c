#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
static int pre_array[100];
static int page_fault[100];
static int number_page_fault = 0;

int ref[100] = { 1, 9, 5, 2, 1, 2, 9, 3, 0, 0, 7 };
int n = 11;
int page_frame = 0;

void insertSequence();
void insertPageFrame();
void copyArray(int arr[], int copy[], int size);
bool notInArray(int element, int array[], int size);
int is_in_prearray(int page, int value);
int farest_element(int *ref, int pivot, int page);
void print_result(int total_page[100][100], int n, int page, int ref[100]);
int nearest_reference(int page, int current_position);
void insertIntoPage(int element,int total_page[100][100], int *current_position, int total_page_position);
int victimPagePosition(int current_position, int total_page[100][100]);
void replacePage(int victimPagePosition, int array[], int replaceWith);
void initArray(int array[100][100]);
void LRU(int ref[], int n, int page);
void OPT(int ref[], int n, int page_frame);

int main()
{
	insertSequence();
	insertPageFrame();
	int select = -1 ;
	while (1)
	{
		printf("--- Page Replacement algorithm ---\n");
		printf("1. FIFO algorithm\n");
		printf("2. OPT algorithm\n");
		printf("3. LRU algorithm\n");
		printf("Algorithm: ");
		scanf("%d", &select);
		printf("--- Page Replacement algorithm ---\n");
		switch (select)
		{
			case 1:
				// Thực hiện giải thuật FIFO và in kết quả
				break;
			case 2:
				// Thực hiện giải thuật OPT và in kết quả
				OPT(ref, n, page_frame);
				break;
			case 3:
				// Thực hiện giải thuật LRU và in kết quả
				LRU(ref, n, page_frame);
				break;
		}
	}
	return 0;
}
void OPT(int ref[], int n, int page_frame)
{
	int total_page[100][100];
	initArray(total_page);
	int page_frame_current_position = 0;
	number_page_fault =0;

	for(int position =0; position< n; position++)
	{
		// Nếu page còn chỗ trống, ghi thêm vào mảng total_page[position] phần tử mới
		if ( page_frame_current_position < page_frame )
		{
			insertIntoPage(ref[position],total_page, &page_frame_current_position, position);
			continue;
		}
		
		// Nếu page hết chỗ trống thì chọn 1 phần tử trong mảng total_page[position]
		copyArray(total_page[position-1],total_page[position],page_frame); //copy từ mảng cũ sang mảng mới
		//Nếu phần tử cần lấy chưa có trong mảng total_page[position] thì
		//chọn 1 victim và thay phần tử victim bằng phần tử cần lấy và tăng giá trị lỗi trang
		if (notInArray(ref[position],total_page[position],page_frame)) { 
			int victim = victimPagePosition(position, total_page);
			replacePage(victim, total_page[position],ref[position]);
			number_page_fault++;
		}
	}

	// In ket qua
	for (int i =0; i < n ; i++) {
		printf("%d\t",ref[i]);
	}
	printf("\n");
	// outer loop for row
	for(int j=0; j<page_frame; j++) {
		// inner loop for column
		for(int i=0; i<n; i++) {
			printf("%d\t", total_page[i][j]);
		}
		printf("\n");
	}
	printf("\nNumber of Page Fault: %d\n", number_page_fault);
}

void LRU(int ref[], int n, int page) 
{
	number_page_fault = 1;
	int total_page[100][100];
	int current_page = 0;
	// Tham chiếu lựa chọn là giá trị đầu tiên trong chuỗi tham chiếu, các giá trị còn lại khởi tạo là 0
	for (int i = 0; i < page; i++) 
	{
		if (i == 0) 
		{
			total_page[i][0] = ref[0];
		} 
		else 
		{
			total_page[i][0] = 0;
		}
	}

	page_fault[0] = 1;
	// Vòng lặp trên mỗi cột bắt đầu từ cột 1 để chọn tham chiếu trong chuỗi tham chiếu
	for (int j = 1; j < n; j++) 
	{
		for (int i = 0; i < page; i++) 
		{
			// Set giá trị cho cột j là cột trước đó
			total_page[i][j] = total_page[i][j - 1];
			// Lưu cột trước đó vào mảng pre_array
			pre_array[i] = total_page[i][j - 1];
		}

		// Kiếm tra xem tham chiếu hiện tại có nằm trong cột trước đó (mảng pre_array) hay không
		// Nếu không thì gán page_fault của tham chiếu đó là 0, nếu có thì tìm vị trí tham chiếu xa nhất trong frame và thay vào frame
		if (is_in_prearray(page, ref[j]) != -1) 
		{
			page_fault[j] = 0;
		}
		else 
		{
			// Nếu vị trí tham chiếu hiện tại nhỏ hơn khung trang thì tăng current_page gán tham chiếu vào frame (frame chưa được sử dụng)
			if (j < page) 
			{
				current_page++;
				total_page[current_page][j] = ref[j];
			} 
			else 
			{
				// Lấy vị trí trong frame cần thay của tham chiếu thứ j
				int pivot = farest_element(ref, j, page);

				// Set giá trị tham chiếu vào frame
				total_page[pivot][j] = ref[j];
			}

			// Gán page_fault của tham chiếu đó là 1
			page_fault[j] = 1;

			// Tăng số lỗi trang
			number_page_fault++;
		}
	}

	// In kết quả ra màn hình
	print_result(total_page, n, page, ref);
}
int victimPagePosition(int current_position, int total_page[100][100])
{
	int nearest_reference_arr[100];
	int victimPosition = 0;
	int maxValue = 0;
	// index nearest reference
	for (int j=0; j < page_frame; j++) {
		nearest_reference_arr[j] = nearest_reference(total_page[current_position][j],current_position);
	}
	// find victim position
	for (int i = 0; i < page_frame; i++) {
		if ( nearest_reference_arr[i] > maxValue ) {
			maxValue = nearest_reference_arr[i];
			victimPosition = i;
		}
	}
	return victimPosition;
}
void replacePage(int victimPagePosition, int array[], int replaceWith)
{
	array[victimPagePosition] = replaceWith;
}

void insertSequence()
{
	int select = -1 ;
	printf("--- Page Replacement algorithm ---\n");
	printf("1. Default referenced sequence\n");
	printf("2. Manual input sequence\n");
	while (select != 1 && select != 2)
	{
		printf("Your choice: ");
		scanf("%d", &select);
		switch(select)
		{
			case 1:
				break;
			case 2:
				printf("Input total number of elements: ");
				scanf("%d", &n);
				printf("Input elements: ");
				for (int i = 0; i < n; i++)
				{
					scanf("%d", &ref[i]);
				}
				break;
			default:
				printf("Chọn không đúng, chọn lại\n");
		}
	}
}
void insertPageFrame()
{
	while (page_frame < 1)
	{
		printf("--- Page Replacement algorithm ---\n");
		printf("Input page frames: ");
		scanf("%d", &page_frame);
	}
}

// Kiểm tra tham chiếu mới có nằm trong cột (frame) trước đó hay không 
// Nếu có trả về vị trí trong frame, ngược lại trả về -1
int is_in_prearray(int page, int value)
{
	for (int i = 0; i < page; i++)
	{
		if (value == pre_array[i])
		{
			return i;
		}
	}
	return -1;
}
void insertIntoPage(int element,int total_page[100][100], int *current_position, int total_page_position)
{
	int i = total_page_position;
	
	if (*current_position!=0) // Nếu vị trí hiện tại khác 0
		copyArray(total_page[i-1],total_page[i],*current_position);
		
	//nếu chưa có trong mảng thì thêm phần tử mới
	if (notInArray(ref[i],total_page[i],*current_position)) { 
		total_page[i][*current_position] = ref[i];
		*current_position = *current_position + 1;
	}
}

// In kết quả ra màn hình
void print_result(int total_page[100][100], int n, int page, int ref[100])
{
	// In chuỗi tham chiếu
	for (int i = 0; i < n; i++)
	{
		printf("%d ", ref[i]);
	}
	printf("\n");

	// In ma trận hai chiều trong quá trình thay trang
	for (int i = 0; i < page; i++)
	{
		for (int j = 0; j < n; j++)
		{
			if (total_page[i][j] != -1)
			{
				printf("%d ", total_page[i][j]);
			}
			else
			{
				printf(" ");
			}
		}
		printf("\n");
	}

	// In lỗi trang
	for (int i = 0; i < n; i++)
	{
		if (page_fault[i] == 1)
		{
			printf("* ");
		}
		else
		{
			printf(" ");
		}
	}
	printf("\nNumber of Page Fault: %d\n", number_page_fault);
}

// Lấy vị trí trong frame có tham chiếu ở xa nhất để thay trang mới
int farest_element(int *ref, int pivot, int page)
{
	int min = pivot;
	// Vị trí trong khung trang sẽ trả về để thay vào
	int idx;
	// Vòng lặp trong khung trang
	for (int i = 0; i < page; i++)
	{
		// Tại mỗi vị trí trong khung trang
		// Thực hiện vòng lặp từ vị trí trước đó của tham chiếu hiện tại trở về 0
		for (int j = pivot - 1; j >= 0; j--)
		{
			// Kiểm tra nếu có vị trí nào trong đó nằm trong chuỗi tham chiếutham chiếu hiện tại
			if (pre_array[i] == ref[j])
			{
				// Nếu có, kiểm tra vị trí này phải nhỏ nhơn min với min ban đầu là vị trí 
				if (j < min)
				{
					// Gán giá trị min là vị trí này
					min = j;
					// Giá trị trả về là khung trang thứ i
					idx = i;
				}
				break;
			}
		}
	}

	return idx;
}

int nearest_reference(int page, int current_position)
{
	for (int i = current_position; i < n; i++) {
		if (ref[i]==page)
		{
			return i;
		}
	}
	return 999;
}
void copyArray(int arr[], int copy[], int size)
{
	// loop to iterate through array
	for (int i = 0; i < size; ++i)
	{
		copy[i] = arr[i];
	}
}
bool notInArray(int element, int array[], int size)
{
	bool notInArray = true;
	for (int i = 0; i < size;i++) {
		if(array[i] == element)
		{
			notInArray = false;
			break;
		}
	}
	return notInArray;
}
void initArray(int array[100][100])
{
	for (int i =0 ; i < 100; i++) {
		for (int j = 0; j < 100; j ++) {
			array[i][j]=-1;
		}
	}
}
